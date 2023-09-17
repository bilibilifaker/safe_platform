#include <iostream>
#include <mysql/mysql.h>
#include "MyJson.h"
#include "MyRSA.h"
using namespace std;

class MySQL{
    private:
        MYSQL *conn;
    public:
        MySQL() {
            MyJson my_json("mysql.json");
            Value my_value = my_json.read_json();
            string IP = my_value["IP"].asString();
            string username = my_value["username"].asString();
            string password = my_value["password"].asString();

            conn = mysql_init(NULL);

            if (mysql_real_connect(conn, IP.c_str(), username.c_str(), password.c_str(), NULL, 0, NULL, 0)) {
                // 连接成功

                // 创建数据库
                if (mysql_query(conn, "CREATE DATABASE IF NOT EXISTS safe_platform")) {
                    std::cerr << "Create database failed: " << mysql_error(conn) << std::endl;
                } 
                else {
                    std::cout << "Database 'safe_platform' created successfully." << std::endl;

                    // 选择数据库
                    if (mysql_query(conn, "USE safe_platform")) {
                        std::cerr << "Select database failed: " << mysql_error(conn) << std::endl;
                    } 
                    else {
                        mysql_query(conn, "DELETE FROM private_key");

                        // 创建表
                        if (mysql_query(conn, "CREATE TABLE IF NOT EXISTS private_key (id INT AUTO_INCREMENT PRIMARY KEY, clientID VARCHAR(32),serverID VARCHAR(32),key_data VARCHAR(2048))")) {
                            std::cerr << "Create table failed: " << mysql_error(conn) << std::endl;
                        } 
                        else {
                            std::cout << "Table 'private_key' created successfully." << std::endl;
                        }
                    }
                }
            } 
            else {
                std::cerr << "Connection failed: " << mysql_error(conn) << std::endl;
            }
        }

        int insert(string clientID,string serverID,string key){
            string sql_insert = "INSERT INTO private_key (clientID, serverID, key_data) VALUES ('";
            sql_insert += clientID;
            sql_insert += "', '";
            sql_insert += serverID;
            sql_insert += "', '";
            sql_insert += key;
            sql_insert += "')";
            if (mysql_query(conn, sql_insert.c_str())) {
                std::cerr << "Insert data failed: " << mysql_error(conn) << std::endl;
                return 0;
            } 
            else {
                std::cout << "Data inserted successfully." << std::endl;
            }
            return 1;
        }

        string get(string clientID,string serverID){
            string sql_get = "SELECT * FROM private_key WHERE clientID = '";
            sql_get += clientID;
            sql_get += "' AND serverID = '";
            sql_get += serverID;
            sql_get += "'";
            if (mysql_query(conn, sql_get.c_str())) {
                std::cerr << "Erase failed: " << mysql_error(conn) << std::endl;
                return 0;
            } 
            else {
                MYSQL_RES *res;
                MYSQL_ROW row;
                res = mysql_store_result(conn);
                row = mysql_fetch_row(res);
                string ret = row[3];
                mysql_free_result(res);
                return ret;
            }
            return "";
        }

        int erase(string clientID, string serverID) {
            string sql_erase = "DELETE FROM private_key WHERE clientID = '";
            sql_erase += clientID;
            sql_erase += "' AND serverID = '";
            sql_erase += serverID;
            sql_erase += "'";
            if (mysql_query(conn, sql_erase.c_str())) {
                std::cerr << "Erase failed: " << mysql_error(conn) << std::endl;
                return 0;
            } 
            else {
                std::cout << "Data erase successfully." << std::endl;
            }
            return 1;
        }

        ~MySQL(){
            mysql_close(conn);
        }
};