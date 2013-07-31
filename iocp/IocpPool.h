#pragma once
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
 namespace pool{
	enum EVENT_TYPE{
		ACCEPT_EVT = 1 << 0, // ��ʼ��
		OUT_EVT = 1 << 1, // ����
		IN_EVT =1 << 2, // ���
		ERROR_EVT = 1 << 3, // ����
	};
	void load()
	{
		WSADATA wsaData;
		int nResult;
		nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	}
	
	void unload()
	{
		
	}
	/**
	 * Ŀ�����
	 */
	class Target{
	public:
		virtual HANDLE getHandle() = 0;
		virtual LPFN_ACCEPTEX getAcceptHandle(){return NULL;}
		virtual HANDLE getPeerHandle() {return -1;}
	};
	/**
	 * �¼�����
	 */
	class EventBase{
	public:
		Target * target; // Ŀ��
		OVERLAPPED     overlapped; 
		EVENT_TYPE eventType;     // ��ʶ�������������(��Ӧ�����ö��)
		virtual void deal() = 0;
		EventBase(Target * target):target(target)
		{
			
		}
		virtual HANDLE getPeerHandle() {return -1;}
	};
	/**
	 * �¼�
	 */
    template<typename TARGET>
	class Event:public EventBase{
    public:
        Event(TARGET *target):EventBase(target)
        {
			reset();
		}
        TARGET * operator->()
        {
            return target;
        }
        bool checkValid()
        {
            return target;
        }
        
		static const unsigned int MAX_BUFFER_LEN = 8192;
		                              // ÿһ���ص�����������ص��ṹ(���ÿһ��Socket��ÿһ����������Ҫ��һ��)              
		WSABUF         m_wsaBuf;                                   // WSA���͵Ļ����������ڸ��ص�������������
		char           buffer[MAX_BUFFER_LEN];                 // �����WSABUF�������ַ��Ļ�����
		           
		DWORD msgLen;
		void reset()
		{
			memset(buffer,0,MAX_BUFFER_LEN);
			memset(&overlapped,0,sizeof(overlapped));  
			m_wsaBuf.buf = buffer;
			m_wsaBuf.len = MAX_BUFFER_LEN;
			eventType     = ERROR_EVT;
			msgLen = 0;
		}
		virtual void deal(){};
    };
	/**
	 * �����¼�
	 */
	template<typename TARGET>
	class InEvent:public Event<TARGET>{
	public:
		InEvent(TARGET *target):Event<TARGET>(target)
        {
			
		}
		void deal()
		{
			DWORD dwFlags = 0;
			DWORD dwBytes = 0;
			WSABUF *p_wbuf   = &m_wsaBuf;
			OVERLAPPED *p_ol = &overlapped;

			reset();
			eventType = IN_EVT;
			int nBytesRecv = WSARecv((SOCKET)target->getHandle(), p_wbuf, 1, &msgLen, &dwFlags, p_ol, NULL );
			if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
			{
				return;
			}
		}
	};
	/**
	 * ����¼�
	 */
	template<typename TARGET>
	class OutEvent:public Event<TARGET>{
	public:
		OutEvent(TARGET *target):Event<TARGET>(target)
		{
			
		}
		void deal()
		{
			DWORD dwFlags = 0;
			DWORD dwBytes = 0;
			WSABUF *p_wbuf   = &m_wsaBuf;
			OVERLAPPED *p_ol = &overlapped;
			reset();
			eventType = OUT_EVT;
			p_wbuf->buf = buffer;
			strcpy(buffer,"OK");
			int nBytesRecv = WSASend((SOCKET)target->getHandle(), p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL );
			if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
			{
				return;
			}
			return;
		}
	};
	/**
	 *  ��ʼ���¼�
	 **/
	template<typename TARGET>
	class AcceptEvent:public Event<TARGET>{
	public:
		AcceptEvent(TARGET *target):Event<TARGET>(target)
		{
			SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);  
			if( INVALID_SOCKET ==  socket)  
			{  
				return;  
			} 
			handle = socket;	
		}
		void deal()
		{
			DWORD dwBytes = 0;  
			eventType = INIT_EVT;  
			WSABUF *p_wbuf   = &m_wsaBuf;
			OVERLAPPED *p_ol = &overlapped;
			// Ͷ��AcceptEx
			if(FALSE == (target->getAcceptHandle())((SOCKET)target->getHandle(),
								handle,
								p_wbuf->buf,0,   
										sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwBytes, p_ol)) 
			{  
				
			}
		}
		SOCKET handle;
		virtual HANDLE getPeerHandle() {return handle;}
	};
	/**
	 * �¼��صķ�װ
	 **/
    class EventPool {
    public:
        EventPool()
        {
           
        }
		bool init()
		{
			poolHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );
			return poolHandle != NULL;
		}
		HANDLE poolHandle;
        void bindEvent(Target *target,int eventType)
		{
			 HANDLE tempHandle = CreateIoCompletionPort((HANDLE)target->getHandle(), poolHandle, (DWORD)target, 0);
			 if (eventType & ACCEPT_EVT)
			 {
				AcceptEvent<Target> *initEvent = new AcceptEvent<Target>(target);
				initEvent->deal();
			 }
			 if (eventType & IN_EVT)
			 {
				InEvent<Target> *inEvent = new InEvent<Target>(target);
				inEvent->deal();
			 }
			 if (eventType & OUT_EVT)
			 {
				OutEvent<Target> *outEvent = new OutEvent<Target>(target);
				outEvent->deal();
			 }
		}

		EventBase* pullEvent()
		{
			// ��ȡһ���¼�
			OVERLAPPED *pOverlapped = NULL;
			Target*target = NULL;
			DWORD dataLen = 0;
			BOOL bReturn = GetQueuedCompletionStatus(
				poolHandle,
				&dataLen,
				(PULONG_PTR)&target,
				&pOverlapped,
				INFINITE
			);
			// ����յ������˳���־����ֱ���˳�
			if ( 0==(DWORD)target)
			{
				return NULL;
			}
			// �ж��Ƿ�����˴���
			if( !bReturn )  
			{  
				return NULL;  
			}  
			else  
			{
				// ��ȡ����Ĳ���
				EventBase* evt = CONTAINING_RECORD(pOverlapped, EventBase, overlapped);  
				return evt;
			}
			return NULL;
		}
    };
 };
 namespace net{
	 class Connection:public pool::Target{
	 public:
		 HANDLE getHandle(){return (HANDLE)socket;}
		 void setHandle(SOCKET socket){this->socket = socket;}
		 void send(){}
		 void read(){}
		 SOCKET socket;
	 };
	 class Server:public pool::Target{
	 public:
		Server(const char *ip,WORD port)
		{
			init(ip,port);
			GUID GuidAcceptEx = WSAID_ACCEPTEX;  
			DWORD dwBytes = 0;  
			if(SOCKET_ERROR == WSAIoctl(
				socket, 
				SIO_GET_EXTENSION_FUNCTION_POINTER, 
				&GuidAcceptEx, 
				sizeof(GuidAcceptEx), 
				&m_lpfnAcceptEx, 
				sizeof(m_lpfnAcceptEx), 
				&dwBytes, 
				NULL, 
				NULL))  
			{  
			}
			
		}
		void init(const char *ip,WORD port)
		{
			struct sockaddr_in ServerAddress;

			// �������ڼ�����Socket����Ϣ

			socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

			// ����ַ��Ϣ
			ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
			ServerAddress.sin_family = AF_INET;
			//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                      
			ServerAddress.sin_addr.s_addr = inet_addr(ip);         
			ServerAddress.sin_port = htons(port);                          

			// �󶨵�ַ�Ͷ˿�
			if (SOCKET_ERROR == bind(socket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) 
			{
				return;
			}
			else
			{
			}

			// ��ʼ���м���
			if (SOCKET_ERROR == listen(socket,SOMAXCONN))
			{
				return;
			}
			else
			{
			}
		}
	
		HANDLE getHandle(){return (HANDLE)socket;}
		SOCKET socket;
		LPFN_ACCEPTEX getAcceptHandle()
		{
			return m_lpfnAcceptEx;
		}
		LPFN_ACCEPTEX                m_lpfnAcceptEx; 
	 };
 };
