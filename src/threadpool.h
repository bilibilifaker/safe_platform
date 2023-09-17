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
// 线程池类
class ThreadPool{
	private:
		Logger* m_log;	// 日志对象

		pthread_mutex_t lock;               /* 用于锁住当前类成员 */
		pthread_mutex_t thread_counter;     /* 记录忙状态线程个数de琐 -- busy_thr_num */
		pthread_cond_t queue_not_empty;     /* 任务队列里不为空时，通知等待任务的线程 */

		pthread_t *threads;                 /* 存放线程池中每个线程的tid。数组 */
		pthread_t adjust_tid;               /* 存管理线程tid */
		TaskQueue *task_queue;				/* 任务队列 */

		int min_thr_num;                    /* 线程池最小线程数 */
		int max_thr_num;                    /* 线程池最大线程数 */
		int live_thr_num;                   /* 当前存活线程个数 */
		int busy_thr_num;                   /* 忙状态线程个数 */
		int wait_exit_thr_num;              /* 要销毁的线程个数 */

		static const int CHECK_TIME;                     /*CHECK_TIME s检测一次*/
		static const int MIN_WAIT_TASK_NUM;              /*如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池*/
		static const int DEFAULT_THREAD_VARY;            /*每次创建和销毁线程的个数*/

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

			/* 初始化互斥琐、条件变量 */
			if (pthread_mutex_init(&(lock), NULL) != 0 || pthread_mutex_init(&(thread_counter), NULL) != 0 || pthread_cond_init(&(queue_not_empty), NULL) != 0){
				m_log->Log("init the lock or cond fail", __FILE__, __LINE__);
			}

			for (int i = 0; i < min_thr_num; i++) {
				// 创建并启动工作的线程
				pthread_create(&threads[i], NULL, threadWorking, this);
				string log = "start thread: " + to_string((unsigned int)threads[i]);
				m_log->Log(log, __FILE__, __LINE__);
			}
			// 创建并启动管理者线程
			pthread_create(&(adjust_tid), NULL, threadManager, this);
		}

		~ThreadPool(){
			if (task_queue){
				delete task_queue;
			}
		}

		// 工作线程回调函数
		static void* threadWorking(void* arg){
			ThreadPool *pool = (ThreadPool *)arg;

			while (true) {
				pthread_mutex_lock(&(pool->lock));

				while (pool->task_queue->taskNumber() == 0) {
					string log = "thread:" + to_string((unsigned int)pthread_self()) + " is waiting";
					pool->m_log->Log(log, __FILE__, __LINE__);
					pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));

					/*清除指定数目的空闲线程，如果要结束的线程个数大于0，结束线程*/
					if (pool->wait_exit_thr_num > 0) {
						pool->wait_exit_thr_num--;

						/*如果线程池里线程个数大于最小值时可以结束当前线程*/
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

				/*从任务队列里获取任务, 是一个出队操作*/
				Task task = pool->task_queue->takeTask();
				/*执行任务*/
				string log = "thread:" + to_string((unsigned int)pthread_self()) + " is start working...";
				pool->m_log->Log(log, __FILE__, __LINE__);
				pthread_mutex_lock(&(pool->thread_counter));                            /*忙状态线程数变量琐*/
				pool->busy_thr_num++;                                                   /*忙状态线程数+1*/
				pthread_mutex_unlock(&(pool->thread_counter));
				// 执行处理动作
				(*(task.function))(task.arg);                                           /*执行回调函数任务*/
				//task.function(task.arg);                                              /*执行回调函数任务*/

				/*任务结束处理*/
				log = "thread:" + to_string((unsigned int)pthread_self()) + " is end working...";
				pool->m_log->Log(log, __FILE__, __LINE__);
				pthread_mutex_lock(&(pool->thread_counter));
				pool->busy_thr_num--;                                       /*处理掉一个任务，忙状态数线程数-1*/
				pthread_mutex_unlock(&(pool->thread_counter));
			}

			pthread_exit(NULL);
			return nullptr;
		}

		// 管理者线程回调函数
		static void* threadManager(void* arg){
			ThreadPool *pool = (ThreadPool*)arg;
			while (true) {
				sleep(CHECK_TIME);                                    /*定时 对线程池管理*/
				pthread_mutex_lock(&(pool->lock));
				size_t queue_size = pool->task_queue->taskNumber();        /* 关注 任务数 */
				int live_thr_num = pool->live_thr_num;                  /* 存活 线程数 */
				pthread_mutex_unlock(&(pool->lock));

				pthread_mutex_lock(&(pool->thread_counter));
				int busy_thr_num = pool->busy_thr_num;                  /* 忙着的线程数 */
				pthread_mutex_unlock(&(pool->thread_counter));

				/* 创建新线程 算法： 任务数大于最小线程池个数, 且存活的线程数少于最大线程个数时 如：30>=10 && 40<100*/
				if (queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num) {
					pthread_mutex_lock(&(pool->lock));
					int add = 0;

					/*一次增加 DEFAULT_THREAD 个线程*/
					for (int i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY && pool->live_thr_num < pool->max_thr_num; i++) {
						if (pool->threads[i] == 0 || !pool->threadIsAlive(pool->threads[i])){
							pthread_create(&(pool->threads[i]), NULL, threadWorking, arg);
							add++;
							pool->live_thr_num++;
						}
					}

					pthread_mutex_unlock(&(pool->lock));
				}

				/* 销毁多余的空闲线程 算法：忙线程X2 小于 存活的线程数 且 存活的线程数 大于 最小线程数时*/
				if ((busy_thr_num * 2) < live_thr_num  &&  live_thr_num > pool->min_thr_num) {
					/* 一次销毁DEFAULT_THREAD个线程, 隨機10個即可 */
					pthread_mutex_lock(&(pool->lock));
					pool->wait_exit_thr_num = DEFAULT_THREAD_VARY;      /* 要销毁的线程数 设置为10 */
					pthread_mutex_unlock(&(pool->lock));

					for (int i = 0; i < DEFAULT_THREAD_VARY; i++) {
						/* 通知处在空闲状态的线程, 他们会自行终止*/
						pthread_cond_signal(&(pool->queue_not_empty));
					}
				}
			}

			return nullptr;
		}

		// 给线程池添加任务
		void addPoolTask(Task &task){
			/*添加任务到任务队列里*/
			task_queue->addTask(task);
			/*添加完任务后，队列不为空，唤醒线程池中 等待处理任务的线程*/
			pthread_cond_signal(&queue_not_empty);
		}

		bool threadIsAlive(pthread_t tid){
			int kill_rc = pthread_kill(tid, 0);
			if (kill_rc == ESRCH) {
				return false;
			}
			return true;
		}

		// 打印日志信息
		void printLog(std::string str){
			std::cout << "File[" << __FILE__ << "], Line[" << __LINE__
				<< "], Infomation[" << str << "]" << std::endl;
		}
};

const int ThreadPool::CHECK_TIME = 10;
const int ThreadPool::MIN_WAIT_TASK_NUM = 1;
const int ThreadPool::DEFAULT_THREAD_VARY = 1;