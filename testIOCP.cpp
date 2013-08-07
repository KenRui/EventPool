#include "pool.h"
/**
 * ͨ���¼��� ʵ�����������
 **/
int main()
{
	pool::init();
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
				pool.bindEvent(conn,pool::IN_EVT|pool::OUT_EVT);
				pool.bindEvent(&server,pool::ACCEPT_EVT);
				printf("add\n");
			}
			if (evt->isOut()) // д
			{
				pool::Event<Connection> *connEvt = (pool::Event<Connection>) evt;
				Connection *conn = connEvt->getTarget();
				if (conn)
				{
					conn->doWrite(evt);
				}
			}
			if (evt->isIn()) // ��
			{
				pool::Event<Connection> *connEvt = (pool::Event<Connection>) evt;
				Connection *conn = connEvt->getTarget();
				if (conn)
				{
					conn->doRead(evt);
				}
			}
			if (evt->isErr()) // ����ɾ��
			{
				pool::Event<Connection> *connEvt = (pool::Event<Connection>) evt;
				Connection *conn = connEvt->getTarget();
				conn->destroy();
				delete conn;
			}
		}
	 }
	 return 1;
}