#include "EpollPool.h"
#include "stdio.h"
 int main()
 {
	pool::load();
	 pool::EventPool pool;
	 pool.init();
	 net::Server server("192.168.123.230",5050);
	 pool.bindEvent(&server,pool::ACCEPT_EVT);

	 while (true) 
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
			if (evt->eventType == pool::ACCEPT_EVT)
			{
				net::Connection *conn =  new net::Connection();
				
				conn->setHandle(evt->getPeerHandle());
				
				pool.bindEvent(&conn,pool::IN_EVT|pool::OUT_EVT); 
				printf("add\n");
			}
			if (evt->eventType & pool::OUT_EVT) 
			{
				pool::Event<net::Connection>* conn 
					= (pool::Event<net::Connection> *)(evt);
				(*conn)->send();
				conn->reset(pool::IN_EVT|pool::OUT_EVT);
			}
			if (evt->eventType & pool::IN_EVT) 
			{
				static int i = 0;
				pool::Event<net::Connection>* conn 
					= (pool::Event<net::Connection> *)(evt);
				(*conn)->read();
				conn->reset(pool::OUT_EVT|pool::IN_EVT);
			}
		}
	 }
	 return 1;
 }
