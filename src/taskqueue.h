#pragma once
#include <queue>
#include <pthread.h>
#include <string>
#include <iostream>
#include "logger.h"

using namespace std;

// ����ṹ��
struct Task {
	Task() {
		function = nullptr;
		arg = nullptr;
	}
	void *(*function)(void *);          /* ����ָ�룬�ص����� */
	void *arg;                          /* ���溯���Ĳ��� */
};

typedef void* (*callback)(void*);

// ���������
class TaskQueue{
	private:
		pthread_mutex_t m_mutex;	// ������
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

