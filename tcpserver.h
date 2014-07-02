#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <resolv.h>
#include <pthread.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "../CBuffer/CBuffer.h"
#include <list>

using namespace std;



class tcpserver;

struct thread_arg
{
	int m_socket;
	tcpserver* m_self;
	CBuffer* m_buffer;
};

class tcpserver
{
public:
	tcpserver(int port);
	void init();
	void execute();
	void halt();
private:
	bool m_running;
	list<CBuffer*> m_bufferList;
	//struct sockaddr_in m_addr;
	int m_port;
	static void *listener(void* _arg);
	static void *connecter(void* _arg);
};



#endif