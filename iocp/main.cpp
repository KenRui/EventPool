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

	 while (true) // ���������߳̿��
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
			if (evt->eventType == pool::ACCEPT_EVT)
			{
				// ��ʼ����connection
				net::Connection *conn =  new net::Connection();

				conn->setHandle(evt->getPeerHandle());

				pool.bindEvent(conn,pool::IN_EVT | pool::OUT_EVT); // �������¼�
			
				printf("add\n");
			}
			if (evt->eventType == pool::OUT_EVT) // д
			{
				printf("write\n");
			}
			if (evt->eventType == pool::IN_EVT) // ��
			{
				printf("read\n");
			}
		}
	 }
	 return 1;
 }