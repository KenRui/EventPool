#pragma hdrstop
#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
//---------------------------------------------------------------------------

#pragma argsused

SOCKET sockClient;
struct sockaddr_in addrServer;
char buf[24];
int n = 0;
int Init();

int main(int argc, char* argv[])
{
    if(Init() != 0)
        goto theend;

    sockClient = socket(AF_INET,SOCK_STREAM,0);
    if(sockClient == INVALID_SOCKET)
    {
        cout<<"socket ʧ��"<<endl;
        WSACleanup();
        goto theend;
    }
    memset(&addrServer,0,sizeof(sockaddr_in));
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_port = htons(5050);
    cout<<"���ӷ�����..."<<endl;
    if(connect(sockClient,(const struct sockaddr *)&addrServer,sizeof(sockaddr)) != 0)
    {
        cout<<"connect ʧ��"<<endl;
        WSACleanup();
        goto theend;
    }
    cout<<"��ʼ���Ͳ��԰�"<<endl;
    memset(buf,0,24);
    while(true)
    {
        sprintf(buf,"��%d����", n);
        cout<<"���ͣ�"<<buf<<endl;
        if(send(sockClient,buf,strlen(buf),0) <= 0)
        {
            cout<<"sendʧ��,�������ӶϿ�"<<endl;
            //break;
            goto theend;
        }
        memset(buf,0,24);

        //���շ����Ӧ��
        if(recv(sockClient,buf,24,0) <= 0)
        {
            cout<<"recvʧ��,�������ӶϿ�"<<endl;
           //break;
           goto theend;
        }
        cout<<"������Ӧ��"<<buf<<endl;
        memset(buf,0,24);

        Sleep(200);
        n++;
    }

    
theend:
    WSACleanup();
    getchar();
    return 0;
}
//---------------------------------------------------------------------------
int Init()
{
    WSAData wsaData;
    if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
    {
        cout<<"WSAStartupʧ��"<<endl;
        return -1;
    }

    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        cout<<"SOCKET�汾����"<<endl;
        WSACleanup();
        return -1;
    }
    return 0;
}

