//
//  main.cpp
//  kqueue
//
//  Created by 季 金龙 on 13-7-30.
//  Copyright (c) 2013年 季 金龙. All rights reserved.
//

#include <iostream>
#include "KqueuePool.h"
#include "stdio.h"
int main(int argc, const char * argv[]){
    
    pool::EventPool pool;
    pool.init();
    net::Server server("127.0.0.1",5050);
    pool::Event<net::Server> *initEvt = new  pool::Event<net::Server>(&server);
    pool.bindEvent(initEvt,pool::IN_EVT);
    
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
				pool.bindEvent(inOutEvt,pool::IN_EVT | pool::OUT_EVT);
				printf("add\n");
			}
			if (evt->eventType & pool::OUT_EVT)
			{
				pool::Event<net::Connection>* conn
                = (pool::Event<net::Connection> *)(evt);
				(*conn)->send();
                evt->delEevnt(pool::OUT_EVT);
			}
			if (evt->eventType & pool::IN_EVT)
			{
				static int i = 0;
				pool::Event<net::Connection>* conn
                = (pool::Event<net::Connection> *)(evt);
				(*conn)->read();
                evt->delEevnt(pool::IN_EVT);
			}
		}
    }
    return 1;
}

