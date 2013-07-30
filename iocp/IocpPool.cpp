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

	 while (true) // ���������߳̿��
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
			if (evt->eventType == pool::INIT_EVT)
			{
				// ��ʼ����connection
				net::Connection *conn =  new net::Connection();
				pool::InitEvent<net::Server>* sevt = (pool::InitEvent<net::Server> *)(evt);
				conn->setHandle(sevt->handle);

				pool::InEvent<net::Connection> *inEvt = new pool::InEvent<net::Connection>(conn);
				pool.bindEvent(inEvt); // �������¼�
				
				pool::OutEvent<net::Connection> *outEvt = new pool::OutEvent<net::Connection>(conn);
				pool.bindEvent(outEvt); // ������¼�
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