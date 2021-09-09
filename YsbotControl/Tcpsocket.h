// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include  <Winsock2.h>
#include "CodeSys\FFMClass.h"

#define  BUF_MAX_LEN		(4096)

class TcpSocket
{
public:
	TcpSocket() throw ();
	virtual ~TcpSocket() throw ();

	virtual bool init_as_server(int port);
	virtual bool init_as_client(char const* addr, int port);
	virtual void close () throw ();

	virtual bool sendto (const char*, unsigned int) throw ();
	virtual bool receive (char*, unsigned int&, unsigned int, int =0) throw ();

	SOCKET socketDescriptor;

	virtual bool started() const throw ();
	virtual bool okay() const throw ();
	virtual bool as_server() const throw ();

protected:
	bool is_connected;
	bool is_okay;
	bool is_server;

	SOCKET creat_socket();

	SOCKADDR_IN partnerAddress;  
	int  adrlen;
	static bool load_socket_lib();
	static bool is_loadlib;

};

class TcpSocketOnce : public TcpSocket
{
public:
	TcpSocketOnce();
	virtual ~TcpSocketOnce();
	bool init_as_client(char const* addr, int port);
	bool init_as_server(int port);
	virtual bool sendto (const char*, unsigned int) throw ();
	virtual bool receive (char*,unsigned int&, unsigned int, int = 0) throw ();

	SOCKET socketConn;

};

class ShareMemeoryConnect : public TcpSocket
{
public:
	ShareMemeoryConnect();
	virtual ~ShareMemeoryConnect();
	bool init_as_client(char const* addr, int port);
	bool init_as_server(int port);
	virtual bool sendto (const char*, unsigned int) throw ();
	virtual bool receive (char*,unsigned int&, unsigned int, int = 0) throw ();
	virtual void close () throw ();

private:
	CFFMServer* memWrite;
	CFFMClient* memRead;
};


#endif