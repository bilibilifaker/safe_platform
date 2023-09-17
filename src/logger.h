#pragma once
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

enum Type { CONSOLE, COMPUTER_FILE };

enum Level { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class Logger
{
    private:
	    Logger(Level level = Level::DEBUG, Type device = Type::CONSOLE){
			m_writer.open("logger.log");
			m_level = level;
			m_device = device;
		}
	    Logger(const Logger& log) = delete;
	    ofstream m_writer;
	    static Logger m_log;
	    Level m_level;
	    Type m_device;

public:
	    static Logger* getInstance(){
			return &m_log;
        }

	    void Log(string text, string file, int line, Level level = INFO){
			bool flag = level >= m_level;
			if (flag){
				if (m_device == CONSOLE){
					// 日志信息控制台输出
					cout << "File[ " << file << " ], Line[ " << line
						<< " ], Infomation[ " << text << " ]" << endl;
				}
				else{
					// 日志信息写文件
					m_writer << "File[ " << file << " ], Line[ " << line
						<< " ], Infomation[ " << text << " ]" << endl;
					// 数据强制刷到磁盘
					m_writer.flush();
				}
			}
		}
        
	    ~Logger(){
            m_writer.close();
        }
};

Logger Logger::m_log;

