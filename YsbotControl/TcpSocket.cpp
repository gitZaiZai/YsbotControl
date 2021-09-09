// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "TcpSocket.h"
#include <iostream>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

bool TcpSocket::is_loadlib = false;

bool TcpSocket::load_socket_lib()
{
	WSADATA wsaData;  
	if( !is_loadlib )
	{  
		if ( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 )  
 		{
			cout << ("Failed to load Winsock!\n");  
			is_loadlib = false;  
 		}
		else
			is_loadlib = true;
	}
 
	return is_loadlib;
}

TcpSocket::TcpSocket() throw () : is_connected(false), is_okay(false)
{
	load_socket_lib();
}

TcpSocket::~TcpSocket()
{
	close ();
}

bool TcpSocket::started() const throw () {
	return is_connected;
}

bool  TcpSocket::okay() const throw () {
	return is_okay;
}

bool TcpSocket::as_server() const throw () {
	return is_connected && is_server;
}

void TcpSocket::close () throw () {
	if (is_connected)
	{
		closesocket(socketDescriptor);
	}
	is_connected=is_okay=false;
	if ( is_loadlib )
	{
		WSACleanup();
 	}
}

bool TcpSocket::init_as_server (int port) throw ()
{
	if ( is_connected )
		closesocket(socketDescriptor);
 
	//创建用于监听的套接字  
	socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  

	partnerAddress.sin_family = AF_INET;  
	partnerAddress.sin_port = htons(port); //1024以上的端口号  
	partnerAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);  
	adrlen = sizeof(SOCKADDR);

	int retVal = bind(socketDescriptor, (LPSOCKADDR)&partnerAddress, sizeof(SOCKADDR_IN));  
	if( retVal == SOCKET_ERROR )
	{  
		printf("Failed bind:%d\n", WSAGetLastError()); 
		closesocket(socketDescriptor);
// 		WSACleanup();
		return false;  
	}  

	if( listen(socketDescriptor,10) == SOCKET_ERROR )
	{  
		printf("Listen failed:%d\n", WSAGetLastError());  
		closesocket(socketDescriptor);
// 		WSACleanup();
		return false;  
	}  

	is_server=true;

	return true;
}
 
bool TcpSocket::init_as_client (const char* hostname, int port) throw () 
{
 	partnerAddress.sin_family = AF_INET;  
	partnerAddress.sin_port = htons(port);  
	partnerAddress.sin_addr.S_un.S_addr = inet_addr(hostname);  

	is_server=false;

	return true;
}

SOCKET TcpSocket::creat_socket()
{
	SOCKET socktmp = SOCKET_ERROR;
	timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	if ( is_server )
	{
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(socketDescriptor, &readSet);

		int ret = select(socketDescriptor+1, &readSet, NULL, NULL, &timeout);
		if ( ret > 0 )
			socktmp = accept( socketDescriptor, (SOCKADDR *) &partnerAddress, &adrlen );
	}
	else
	{
		socktmp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	if( SOCKET_ERROR == socktmp )
	{  
  		printf("Socket() error:%d\n", WSAGetLastError());
// 		WSACleanup(); // 多个TCP连接，若是有一个出现错误，导致其他TCP连接也断开。
		is_connected = false;
		return SOCKET_ERROR;    
	} 

 	int ret = setsockopt(socktmp,SOL_SOCKET,SO_SNDTIMEO,(char*)(&timeout),sizeof(timeout));

	// 向服务器发出连接请求  
	if( !is_server)
	{  
		ETIMEDOUT;
		int nret = connect(socktmp, (struct  sockaddr*)&partnerAddress, sizeof(partnerAddress));
		if ( nret == SOCKET_ERROR )
		{
  			printf("Connect failed:%d\n", WSAGetLastError()); 
			closesocket(socktmp);
// 			WSACleanup();
			is_connected = false;
			return SOCKET_ERROR;  
		}
	}
// 	else
// 		printf("Accept client IP:[%s]\n", inet_ntoa(partnerAddress.sin_addr));

	return socktmp;
}

bool TcpSocket::sendto(const char* buffer, unsigned int buflen) throw ()
{
	SOCKET sockcurr = creat_socket();
	if ( sockcurr == SOCKET_ERROR )
	{
		cout << " error kim \n";
		return false;
	}

	is_connected = true;
	int ret = send( sockcurr, buffer, buflen, 0 );  
	if ( ret == SOCKET_ERROR ) 
	{
		printf("send failed\n"); 
		is_okay = false;
		return false;  
	}

	is_okay = true;

	closesocket(sockcurr);

	return true;
}

bool  TcpSocket::receive (char* buffer, unsigned int& buflen, unsigned int maxbuflen,int timeout) throw ()
{
	//  		 
	SOCKET sockcurr = creat_socket();
	if ( sockcurr == SOCKET_ERROR )
		return false;

	is_connected = true;

	int buflen2 = -1;

	struct sockaddr_in senderAddress;
	int senderAddressLength =  (int) ( sizeof(senderAddress) );
	if ( is_server )
	{
		buflen2 = recvfrom(sockcurr, buffer, maxbuflen, 0,
			(struct sockaddr *) &senderAddress,
			&senderAddressLength);
	}
	else
		buflen2 = recv( sockcurr, buffer, maxbuflen, 0 );

	buffer[buflen2] = '\0';

	if ( buflen2 < 0 ) 
	{
		is_okay=false;
		return false;
	}

	if (is_server && (senderAddress.sin_addr.s_addr!=partnerAddress.sin_addr.s_addr || senderAddress.sin_port!=partnerAddress.sin_port)) 
	{
		partnerAddress.sin_addr.s_addr = senderAddress.sin_addr.s_addr;
		partnerAddress.sin_port = senderAddress.sin_port;
	}
	is_okay = true;
	buflen  = buflen2;

	closesocket(sockcurr);  


	return true;

}

//////////////////////////////////////////////////////////////////////////

TcpSocketOnce::TcpSocketOnce()
{
	socketConn = INVALID_SOCKET;
}

TcpSocketOnce::~TcpSocketOnce()
{
	closesocket(socketConn);
}

bool TcpSocketOnce::init_as_server(int port)
{
	bool ret = TcpSocket::init_as_server(port);
	if ( ret )
	{
		timeval timeout;
		timeout.tv_sec = 100;
		timeout.tv_usec = 0;

		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(socketDescriptor, &readSet);

		int ret = select(socketDescriptor+1, &readSet, NULL, NULL, &timeout);
		if ( ret > 0 )
		{
			socketConn = accept( socketDescriptor, (SOCKADDR *) &partnerAddress, &adrlen );
		}

		if( SOCKET_ERROR == socketConn )
		{  
			printf("Socket() error:%d\n", WSAGetLastError());
			WSACleanup();
			is_connected = false;
			return false;    
		} 

		ret = setsockopt(socketConn,SOL_SOCKET,SO_SNDTIMEO,(char*)(&timeout),sizeof(timeout));
	}
	else
		return false;

	return true;
}

bool TcpSocketOnce::init_as_client(char const* addr, int port)
{
	bool ret = TcpSocket::init_as_client(addr,port);
	if ( ret )
	{
		socketDescriptor = TcpSocket::creat_socket();
		if ( socketDescriptor == SOCKET_ERROR ||
			socketDescriptor == INVALID_SOCKET )
			return false;
	}
	else
		return false;

	return true;
}

bool TcpSocketOnce::sendto(const char* buffer, unsigned int buflen)
{
	SOCKET sockcurr = is_server ? socketConn : socketDescriptor;
	if ( sockcurr == SOCKET_ERROR )
		return false;

	is_connected = true;
	int ret = send( sockcurr, buffer, buflen, 0 );  
	if ( ret == SOCKET_ERROR ) 
	{
		printf("send failed\n"); 
		is_okay = false;
		return false;  
	}

	is_okay = true;


	return true;
}

bool TcpSocketOnce::receive(char* buffer,unsigned int& buflen, unsigned int maxbuflen,int timeout) throw ()
{
	SOCKET sockcurr = is_server ? socketConn : socketDescriptor;
	if ( sockcurr == SOCKET_ERROR )
		return false;

	is_connected = true;

	int buflen2 = -1;

	struct sockaddr_in senderAddress;
	int senderAddressLength =  (int) ( sizeof(senderAddress) );
	if ( is_server )
	{
		buflen2 = recvfrom(sockcurr, buffer, maxbuflen, 0,
			(struct sockaddr *) &senderAddress,
			&senderAddressLength);

	}
	else
		buflen2 = recv( sockcurr, buffer, maxbuflen, 0 );

	buffer[buflen2] = '\0';

	if ( buflen2 < 0 ) 
	{
		is_okay=false;
		return false;
	}

	if (is_server && (senderAddress.sin_addr.s_addr!=partnerAddress.sin_addr.s_addr || senderAddress.sin_port!=partnerAddress.sin_port)) 
	{
		partnerAddress.sin_addr.s_addr = senderAddress.sin_addr.s_addr;
		partnerAddress.sin_port = senderAddress.sin_port;
	}
	is_okay = true;
	buflen  = buflen2;


	return true;
}

//////////////////////////////////////////////////////////////////////////

ShareMemeoryConnect::ShareMemeoryConnect()
{
	memRead = NULL;
	memWrite = NULL;
}

ShareMemeoryConnect::~ShareMemeoryConnect()
{
	if ( memWrite )
	{
		delete memWrite;
		memWrite = NULL;
	}

	if ( memRead )
	{
		delete memRead;
		memRead = NULL;
	}
}

bool ShareMemeoryConnect::init_as_server(int port)
{

	return true;
}

bool ShareMemeoryConnect::init_as_client(char const* addr, int port)
{
	memWrite = new CFFMServer(DEFAULT_FILENAME,DEFAULT_MAPNAME,DEFAULT_MAPSIZE);

	memRead  = new CFFMClient(FILE_MAP_READ,SERVER_MAPNAME,1024);

	ZeroMemory((char*)memWrite->GetBuffer(), memWrite->GetSize() * sizeof(char));

	return true;
}

bool ShareMemeoryConnect::sendto(const char* buffer, unsigned int buflen)
{
	if ( memWrite )
	{
		memWrite->WriteBuffer( buffer, buflen );
	}
	return true;
}

bool ShareMemeoryConnect::receive(char* buffer,unsigned int& buflen, unsigned int maxbuflen,int timeout) throw ()
{
	unsigned int dataSize =sizeof(Ysbot::IECServerData)+sizeof(Ysbot::DATA_HEADER)-1; // BUF_MAX_LEN;
	buflen = dataSize;
	if ( memRead )
	{
		memRead->ReadBuffer( buffer, dataSize );
	} 
	return true;
}

void ShareMemeoryConnect::close()
{
	;
}