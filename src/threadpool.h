#pragma once
#include "threadpool.h"
#include "taskqueue.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "logger.h"
#define LOG
// �̳߳���
class ThreadPool{
	private:
		Logger* m_log;	// ��־����

		pthread_mutex_t lock;               /* ������ס��ǰ���Ա */
		pthread_mutex_t thread_counter;     /* ��¼æ״̬�̸߳���de�� -- busy_thr_num */
		pthread_cond_t queue_not_empty;     /* ��������ﲻΪ��ʱ��֪ͨ�ȴ�������߳� */

		pthread_t *threads;                 /* ����̳߳���ÿ���̵߳�tid������ */
		pthread_t adjust_tid;               /* ������߳�tid */
		TaskQueue *task_queue;				/* ������� */

		int min_thr_num;                    /* �̳߳���С�߳��� */
		int max_thr_num;                    /* �̳߳�����߳��� */
		int live_thr_num;                   /* ��ǰ����̸߳��� */
		int busy_thr_num;                   /* æ״̬�̸߳��� */
		int wait_exit_thr_num;              /* Ҫ���ٵ��̸߳��� */

		static const int CHECK_TIME;                     /*CHECK_TIME s���һ��*/
		static const int MIN_WAIT_TASK_NUM;              /*���queue_size > MIN_WAIT_TASK_NUM ����µ��̵߳��̳߳�*/
		static const int DEFAULT_THREAD_VARY;            /*ÿ�δ����������̵߳ĸ���*/

	public:
		ThreadPool(int min = 2, int max = 10){

			task_queue = new TaskQueue;
			m_log = Logger::getInstance();

			min_thr_num = min;
			max_thr_num = max;
			busy_thr_num = 0;
			live_thr_num = min_thr_num;

			threads = new pthread_t[sizeof(pthread_t) * max_thr_num];
			if (threads == NULL) {
				m_log->Log("malloc threads fail", __FILE__, __LINE__);
			}
			memset(threads, 0, sizeof(pthread_t) * max_thr_num);

			/* ��ʼ������������������ */
			if (pthread_mutex_init(&(lock), NULL) != 0 || pthread_mutex_init(&(thread_counter), NULL) != 0 || pthread_cond_init(&(queue_not_empty), NULL) != 0){
				m_log->Log("init the lock or cond fail", __FILE__, __LINE__);
			}

			for (int i = 0; i < min_thr_num; i++) {
				// �����������������߳�
				pthread_create(&threads[i], NULL, threadWorking, this);
				string log = "start thread: " + to_string((unsigned int)threads[i]);
				m_log->Log(log, __FILE__, __LINE__);
			}
			// �����������������߳�
			pthread_create(&(adjust_tid), NULL, threadManager, this);
		}

		~ThreadPool(){
			if (task_queue){
				delete task_queue;
			}
		}

		// �����̻߳ص�����
		static void* threadWorking(void* arg){
			ThreadPool *pool = (ThreadPool *)arg;

			while (true) {
				pthread_mutex_lock(&(pool->lock));

				while (pool->task_queue->taskNumber() == 0) {
					string log = "thread:" + to_string((unsigned int)pthread_self()) + " is waiting";
					pool->m_log->Log(log, __FILE__, __LINE__);
					pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));

					/*���ָ����Ŀ�Ŀ����̣߳����Ҫ�������̸߳�������0�������߳�*/
					if (pool->wait_exit_thr_num > 0) {
						pool->wait_exit_thr_num--;

						/*����̳߳����̸߳���������Сֵʱ���Խ�����ǰ�߳�*/
						if (pool->live_thr_num > pool->min_thr_num) 	{
							log = "thread:" + to_string((unsigned int)pthread_self()) + " is exiting";
							pool->m_log->Log(log, __FILE__, __LINE__);
							pool->live_thr_num--;
							pthread_mutex_unlock(&(pool->lock));
							pthread_exit(NULL);
						}
					}
				}
				pthread_mutex_unlock(&(pool->lock));

				/*������������ȡ����, ��һ�����Ӳ���*/
				Task task = pool->task_queue->takeTask();
				/*ִ������*/
				string log = "thread:" + to_string((unsigned int)pthread_self()) + " is start working...";
				pool->m_log->Log(log, __FILE__, __LINE__);
				pthread_mutex_lock(&(pool->thread_counter));                            /*æ״̬�߳���������*/
				pool->busy_thr_num++;                                                   /*æ״̬�߳���+1*/
				pthread_mutex_unlock(&(pool->thread_counter));
				// ִ�д�����
				(*(task.function))(task.arg);                                           /*ִ�лص���������*/
				//task.function(task.arg);                                              /*ִ�лص���������*/

				/*�����������*/
				log = "thread:" + to_string((unsigned int)pthread_self()) + " is end working...";
				pool->m_log->Log(log, __FILE__, __LINE__);
				pthread_mutex_lock(&(pool->thread_counter));
				pool->busy_thr_num--;                                       /*�����һ������æ״̬���߳���-1*/
				pthread_mutex_unlock(&(pool->thread_counter));
			}

			pthread_exit(NULL);
			return nullptr;
		}

		// �������̻߳ص�����
		static void* threadManager(void* arg){
			ThreadPool *pool = (ThreadPool*)arg;
			while (true) {
				sleep(CHECK_TIME);                                    /*��ʱ ���̳߳ع���*/
				pthread_mutex_lock(&(pool->lock));
				size_t queue_size = pool->task_queue->taskNumber();        /* ��ע ������ */
				int live_thr_num = pool->live_thr_num;                  /* ��� �߳��� */
				pthread_mutex_unlock(&(pool->lock));

				pthread_mutex_lock(&(pool->thread_counter));
				int busy_thr_num = pool->busy_thr_num;                  /* æ�ŵ��߳��� */
				pthread_mutex_unlock(&(pool->thread_counter));

				/* �������߳� �㷨�� ������������С�̳߳ظ���, �Ҵ����߳�����������̸߳���ʱ �磺30>=10 && 40<100*/
				if (queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num) {
					pthread_mutex_lock(&(pool->lock));
					int add = 0;

					/*һ������ DEFAULT_THREAD ���߳�*/
					for (int i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY && pool->live_thr_num < pool->max_thr_num; i++) {
						if (pool->threads[i] == 0 || !pool->threadIsAlive(pool->threads[i])){
							pthread_create(&(pool->threads[i]), NULL, threadWorking, arg);
							add++;
							pool->live_thr_num++;
						}
					}

					pthread_mutex_unlock(&(pool->lock));
				}

				/* ���ٶ���Ŀ����߳� �㷨��æ�߳�X2 С�� �����߳��� �� �����߳��� ���� ��С�߳���ʱ*/
				if ((busy_thr_num * 2) < live_thr_num  &&  live_thr_num > pool->min_thr_num) {
					/* һ������DEFAULT_THREAD���߳�, �S�C10������ */
					pthread_mutex_lock(&(pool->lock));
					pool->wait_exit_thr_num = DEFAULT_THREAD_VARY;      /* Ҫ���ٵ��߳��� ����Ϊ10 */
					pthread_mutex_unlock(&(pool->lock));

					for (int i = 0; i < DEFAULT_THREAD_VARY; i++) {
						/* ֪ͨ���ڿ���״̬���߳�, ���ǻ�������ֹ*/
						pthread_cond_signal(&(pool->queue_not_empty));
					}
				}
			}

			return nullptr;
		}

		// ���̳߳��������
		void addPoolTask(Task &task){
			/*����������������*/
			task_queue->addTask(task);
			/*���������󣬶��в�Ϊ�գ������̳߳��� �ȴ�����������߳�*/
			pthread_cond_signal(&queue_not_empty);
		}

		bool threadIsAlive(pthread_t tid){
			int kill_rc = pthread_kill(tid, 0);
			if (kill_rc == ESRCH) {
				return false;
			}
			return true;
		}

		// ��ӡ��־��Ϣ
		void printLog(std::string str){
			std::cout << "File[" << __FILE__ << "], Line[" << __LINE__
				<< "], Infomation[" << str << "]" << std::endl;
		}
};

const int ThreadPool::CHECK_TIME = 10;
const int ThreadPool::MIN_WAIT_TASK_NUM = 1;
const int ThreadPool::DEFAULT_THREAD_VARY = 1;