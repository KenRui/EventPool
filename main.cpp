/**
 * ����
 */
 class Connection{
 
 };
 /**
  * �¼�������
  **/
 class EventCallback{
 public:
	
 };
 /**
  * ��������
  **/
 int main()
 {
	ThreadNetworkManager<EventCallback,Connection,Decoder> pool;
	Server server("",1999);
	pool.bindServer(&server);	
 }