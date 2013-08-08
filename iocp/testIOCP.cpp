#include "pool.h"
/**
 * ͨ���¼��� ʵ�����������
 **/
int main()
{
	 pool::load();
	 pool::EventPool pool;
	 pool.init();
	 net::Server server("127.0.0.1",5050);
	 for (int i = 0; i < 5;i++) // ������5������
	 {
		 pool.bindEvent(&server,pool::ACCEPT_EVT);
	 }

	 while (true) // ���������߳̿��
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
			if (evt->isAccept()) // �½�connection
			{
				// ��ʼ����connection
				net::Connection *conn =  new net::Connection();
				conn->setHandle((SOCKET)evt->getPeerHandle());
				conn->sendCmd((void*)"hello",4);
				pool.bindEvent(conn,pool::IN_EVT|pool::OUT_EVT);
				pool.bindEvent(&server,pool::ACCEPT_EVT);
				printf("add\n");
				continue;
			}
			if (evt->isErr()) // ����ɾ��
			{
				pool::Event<net::Connection> *connEvt = (pool::Event<net::Connection>*) evt;
				net::Connection *conn = (net::Connection*) connEvt->target;
				if (conn)
				{
					conn->destroy();
					delete conn;
				}
				printf("del\n");
				continue;
			}
			if (evt->isOut()) // д
			{
				pool::Event<net::Connection> *connEvt = (pool::Event<net::Connection>*) evt;
				if (connEvt->target)
				{
					(*connEvt)->doSend(evt);
				}
				printf("out\n");
			}
			if (evt->isIn()) // ��
			{
				pool::Event<net::Connection> *connEvt = (pool::Event<net::Connection>*) evt;
				if (connEvt->target)
				{
					(*connEvt)->doRead(evt);
				}
				(*connEvt)->sendCmd((void*)"hello",4);
				//printf("in\n");
			}
			
		}
	 }
	 return 1;
}