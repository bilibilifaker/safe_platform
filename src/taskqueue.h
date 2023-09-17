#pragma once
#include <queue>
#include <pthread.h>
#include <string>
#include <iostream>
#include "logger.h"

using namespace std;

// 任务结构体
struct Task {
	Task() {
		function = nullptr;
		arg = nullptr;
	}
	void *(*function)(void *);          /* 函数指针，回调函数 */
	void *arg;                          /* 上面函数的参数 */
};

typedef void* (*callback)(void*);

// 任务队列类
class TaskQueue{
	private:
		pthread_mutex_t m_mutex;	// 互斥锁
		queue<Task> m_queue;

	public:
		TaskQueue(){
			pthread_mutex_init(&m_mutex, NULL);
		}

		~TaskQueue(){
			pthread_mutex_destroy(&m_mutex);
		}

		void addTask(Task &task){
			pthread_mutex_lock(&m_mutex);
			m_queue.push(task);
			pthread_mutex_unlock(&m_mutex);
		}
	
		void addTask(callback func, void* arg){
			Task t;
			t.arg = arg;
			t.function = func;
			pthread_mutex_lock(&m_mutex);
			m_queue.push(t);
			pthread_mutex_unlock(&m_mutex);
		}

		size_t taskNumber(){
			return m_queue.size();
		}

		Task takeTask(){
			if (m_queue.size() > 0){
				pthread_mutex_lock(&m_mutex);
				Task t = m_queue.front();
				m_queue.pop();
				pthread_mutex_unlock(&m_mutex);
				return t;
			}
			return Task();
		}

		void printLog(string str){
			cout << "File[" << __FILE__ << "], Line[" << __LINE__
				<< "], Infomation[" << str << "]" << endl;
		}

};

