#include "pool.h"
#include "thread.h"
class ThreadNetworkManager; 
/**
 * 接受者
 */
class Accept{
	Accept(ThreadNetworkManager *tnm):tnm(tnm)
	{
		valid = false;
	}
	void run()
	{
		while (valid)
		{
			EventBase *event = pool.pullEvent();
			if (event)
			{
				if (event->isIn())
				{
					tnm->theCheck()->addTarget(conn);
				}
				if (event->isOut())
				{
				
				}
				if (event->isErr())
				{
					evt->doDelete();
					tnm->theRecycle()->addConntection(conn);
				}
			}
		}
	}
	void addTarget(Target *target)
	{
		pool.bindEvent(target,ACCEPT_EVT);
	}
	
	void stop()
	{
		valid = false;
	}
	bool valid;
	
	EventPool pool;
	
	ThreadNetworkManager *tnm;
};
/**
 * 工作者
 */
class Worker:public Thread{
public:
	Worker():(ThreadNetworkManager *tnm):tnm(tnm)
	{
		valid = false;
	}
	void run()
	{
		while (valid)
		{
			EventBase *event = pool.pullEvent();
			if (event)
			{
				if (event->isIn())
				{
					logic->doRead(conn);
					evt->doRead();
				}
				if (event->isOut())
				{
					logic->doWriter(conn);
					evt->doWrite();
				}
				if (event->isErr())
				{
					evt->doDelete();
					tnm->theRecycle()->addTarget(conn);
				}
			}
		}
	}
	void addTarget(Connection *connection)
	{
		pool.bindEvent(connection,IN_EVT|OUT_EVT);
	}
	void stop()
	{
		valid = false;
	}
	bool valid;
	
	EventPool pool;
};
/**
 * 检查者
 */
class Checker:public Thread{
public:
	Checker():(ThreadNetworkManager *tnm):tnm(tnm)
	{
		valid = false;
	}
	void run()
	{
		while (valid)
		{
			EventBase *event = pool.pullEvent();
			if (event)
			{
				if (event->isIn())
				{
					int OK = logic->check(conn,event);
					if (OK)
					{
						event->delEvent(IN_EVT|OUT_EVT);
						tnm->theWorker->addTarget(conn);
					}
					conn->doRead();
				}
				if (event->isOut())
				{
					conn->doWrite();
				}
				if (event->isErr())
				{
					event->doDelete(); // 删除
					tnm->theRecycle()->addTarget(conn);
				}
			}
		}
	}
	void addTarget(Connection *connection)
	{
		pool.bindEvent(connection,IN_EVT|OUT_EVT);
	}
	void stop()
	{
		valid = false;
	}
	bool valid;
	EventPool pool;
};
/**
 * 回收者
 **/
class Recycle:public Thread{
public:
	Recycle()
	{
		valid = false;
	}
	void run()
	{
		while (valid)
		{
			for (;;)
			{
				delete connection;
			}
		}
	}
	void addTarget(Connection *connection)
	{
		conns.push_back(connection);
	}
	void stop()
	{
		valid = false;
	}
	std::list<Connection*> conns;
	bool valid;
};
/**
 * 线程管理器
 */
class ThreadNetworkManager{
public:
	void bindServer(Server *sever)
	{
		acceptPool.addTarget(server);
	}
	void init(unsinged int checknum = 1,unsigned int worknum=2,unsigned int recyclenum=2)
	{
		for (unsigned int index = 0; index < checknum;index++)
		{
			Check *check = new Check(this);
			check->start();
			checkPool->add(check);
		}
		for (unsigned int index = 0; index < worknum;index++)
		{
			Worker *worker = new Worker(this);
			worker->start();
			workPool->add(worker);
		}
	}
	ThreadGroup workPool;
	ThreadGroup checkPool;
	ThreadGroup delPool;
	Accept acceptPool; // 接受池
	
	Woker * theWork()
	{
		static unsigned int workindex = 0;
		if (workindex < workPool.size())
		{
			unsigned index = workindex;
			workindex = (workindex + 1) % workPool.size()
			return workPool.get(index);
		}
		return NULL;
	}
	Check * theCheck();
	Recycle* theRecycle();
	
	void destroy()
	{
		stopAll();
		joinAll();
	}
};