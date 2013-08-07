#include "pool.h"
/**
 * 通用事件池 实现网络服务器
 **/
int main()
{
	pool::init();
	 pool::EventPool pool;
	 pool.init();
	 net::Server server("127.0.0.1",5050);
	 for (int i = 0; i < 5;i++) // 允许有5个连接
	 {
		 pool.bindEvent(&server,pool::ACCEPT_EVT);
	 }

	 while (true) // 服务器单线程框架
	 {
		pool::EventBase *evt = pool.pullEvent();
		if (evt)
		{
			if (evt->isAccept()) // 新建connection
			{
				// 开始创建connection
				net::Connection *conn =  new net::Connection();
				pool.bindEvent(conn,pool::IN_EVT|pool::OUT_EVT);
				pool.bindEvent(&server,pool::ACCEPT_EVT);
				printf("add\n");
			}
			if (evt->isOut()) // 写
			{
				pool::Event<Connection> *connEvt = (pool::Event<Connection>) evt;
				Connection *conn = connEvt->getTarget();
				if (conn)
				{
					conn->doWrite(evt);
				}
			}
			if (evt->isIn()) // 读
			{
				pool::Event<Connection> *connEvt = (pool::Event<Connection>) evt;
				Connection *conn = connEvt->getTarget();
				if (conn)
				{
					conn->doRead(evt);
				}
			}
			if (evt->isErr()) // 出错删除
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