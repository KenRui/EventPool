#include "EpollPool.h"
#include "stdio.h"
 int main()
 {

	 pool::EventPool pool;
	 pool.init();
	 net::Server server("192.168.123.230",5050);
	 pool::Event<net::Server> *initEvt = new  pool::Event<net::Server>(&server);
	 initEvt->set(pool::INIT_EVT);
	 pool.bindEvent(initEvt);

	 while (true) 
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
			if (evt->eventType == pool::INIT_EVT)
			{
				net::Connection *conn =  new net::Connection();
				pool::Event<net::Server>* sevt = (pool::Event<net::Server> *)(evt);
				conn->setHandle((*sevt)->accept());

				pool::Event<net::Connection> *inOutEvt = new pool::Event<net::Connection>(conn);
				inOutEvt->set(pool::IN_EVT|pool::OUT_EVT);
				pool.bindEvent(inOutEvt); 
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
