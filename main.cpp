/**
 * 连接
 */
 class Connection{
 
 };
 /**
  * 事件处理器
  **/
 class EventCallback{
 public:
	
 };
 /**
  * 启动函数
  **/
 int main()
 {
	ThreadNetworkManager<EventCallback,Connection,Decoder> pool;
	Server server("",1999);
	pool.bindServer(&server);	
 }