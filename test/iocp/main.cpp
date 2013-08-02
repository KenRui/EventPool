#include "IocpPool.h"
#include "stdio.h"
 int main()
 {
	 pool::load();
	 pool::EventPool pool;
	 pool.init();
	 net::Server server("127.0.0.1",5050);
	 for (int i = 0; i < 10;i++)
	 {
		 pool.bindEvent(&server,pool::ACCEPT_EVT);
	 }

	 while (true) // 服务器单线程框架
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
			if (evt->eventType == pool::ACCEPT_EVT)
			{
				// 开始创建connection
				net::Connection *conn =  new net::Connection();

				conn->setHandle(evt->getPeerHandle());

				pool.bindEvent(conn,pool::IN_EVT | pool::OUT_EVT); // 绑定输入事件
			
				printf("add\n");
			}
			if (evt->eventType == pool::OUT_EVT) // 写
			{
				printf("write\n");
			}
			if (evt->eventType == pool::IN_EVT) // 读
			{
				printf("read\n");
			}
		}
	 }
	 return 1;
 }