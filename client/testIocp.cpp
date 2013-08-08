#include "pool.h"
/**
 * ͨ���¼��� ʵ�����������
 **/
int main()
{
	 pool::load();
	 pool::EventPool pool;
	 pool.init();
	 net::Client client("127.0.0.1",5050);
	
	 pool.bindEvent(&client,pool::IN_EVT|pool::OUT_EVT);
	 while (true) // ���������߳̿��
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
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
				(*connEvt)->sendCmd((void*)"hello",6);
				//printf("in\n");
			}
			
		}
	 }
	 return 1;
}