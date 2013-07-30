#include "IocpPool.h"
#include "stdio.h"
 int main()
 {

	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	 pool::EventPool pool;
	 pool.init();
	 net::Server server("127.0.0.1",5050);
	 for (int i = 0; i < 10;i++)
	 {
		 pool::InitEvent<net::Server> *initEvt = new  pool::InitEvent<net::Server>(&server);
		 pool.bindEvent(initEvt);
	 }

	 while (true) // 服务器单线程框架
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
			if (evt->eventType == pool::INIT_EVT)
			{
				// 开始创建connection
				net::Connection *conn =  new net::Connection();
				pool::InitEvent<net::Server>* sevt = (pool::InitEvent<net::Server> *)(evt);
				conn->setHandle(sevt->handle);

				pool::InEvent<net::Connection> *inEvt = new pool::InEvent<net::Connection>(conn);
				pool.bindEvent(inEvt); // 绑定输入事件
				
				pool::OutEvent<net::Connection> *outEvt = new pool::OutEvent<net::Connection>(conn);
				pool.bindEvent(outEvt); // 绑定输出事件
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