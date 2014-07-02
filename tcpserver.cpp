
#include "tcpserver.h"

#include <unistd.h>

#include <cstring>
#include <cstdlib>


using namespace std;

#define BUF_SIZE 256


tcpserver::tcpserver(int port)
{
	m_port = htons(port);
	m_running = true;
	m_bufferList.clear();
}

void
tcpserver::init()
{
	struct sockaddr_in addr;
	int sd;
	/*--- create socket ---*/
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if ( sd < 0 )
	{
		perror("socket"); 
		abort();
	}
	/*--- enable reuse address in case of restart ---*/
	int on = 1;
	if ( (setsockopt(sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) ) < 0 )
	{
		perror("setsockopt"); 
		abort();
	}
	/*--- bind port/address to socket ---*/
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = m_port;
	addr.sin_addr.s_addr = INADDR_ANY;      /* any interface */
	if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
	{
		perror("bind"); 
		abort();
	}
	
	/*--- make into listener with 10 slots ---*/
	if ( listen(sd, 10) != 0 )
	{
		perror("bind"); 
		abort();
	}
	
	/*--- start pthread listener ---*/
	pthread_t listener_thread;
	thread_arg* listener_thread_arg = new thread_arg();
	listener_thread_arg->m_socket = sd;
	listener_thread_arg->m_self = this;
	pthread_create(&listener_thread, 0, &listener, listener_thread_arg);       /* start thread */
	printf("Server socket is %d\n",sd);
	return;
}

void
*tcpserver::listener(void* _arg)
{
	thread_arg* arg = (thread_arg*)_arg;
	int sd = arg->m_socket;
	tcpserver* self = arg->m_self;
	delete arg;
	int csd;
	pthread_t connecter_thread;
	/*--- begin waiting for connections ---*/
	while(self->m_running)
	{
		csd = accept(sd, NULL, NULL);
		if (csd > 0 )
		{
			thread_arg* carg = new thread_arg();
			printf("connected with socket %d\n",csd);
			carg->m_socket = csd;
			carg->m_self = self;
			
			CBuffer* buffer = new CBuffer();
			buffer->setSocket(csd);
			self->m_bufferList.push_back(buffer);
			carg->m_buffer = buffer;
			
			pthread_create(&connecter_thread, 0, &connecter, carg);     /* start thread */
		}
	}
	close(sd);
	pthread_exit(NULL);
}

void
*tcpserver::connecter(void* _arg)
{
	thread_arg* arg = (thread_arg*)_arg;
	int csd = arg->m_socket;
	tcpserver* self = arg->m_self;
	CBuffer* cbuffer = arg->m_buffer;
	delete arg;
	char buffer[BUF_SIZE];
	int n;
	//char* rbuf;
	char* addr = new char[256];
	struct sockaddr_in remote;
	socklen_t len=sizeof(sockaddr_in);
	if (getpeername(csd,(struct sockaddr *)&remote,&len) < 0)
	{
		perror("getpeername");
	} else {
		
		sprintf(addr,"%s %u",
				inet_ntoa(remote.sin_addr),
				ntohs(remote.sin_port));
		printf("address=%s\n",addr);
		cbuffer->setAddress(addr);
	}
	
	
	memset(buffer,'\0', BUF_SIZE);
	do 
	{
		n = read(csd, buffer, BUF_SIZE );
		//printf("<%d\n",n);
		if (n>0)
		{
			cbuffer->write(buffer,n);
			//int m = cbuffer->read(rbuf);
			//if (m>0)
			//{
			//	write(csd,rbuf,m);
			//}
			//write(csd, buffer,n);
			//printf("received:\n%s",buffer);
		}
		memset(buffer,'\0', BUF_SIZE);
	}
	while(self->m_running && (n > 0));
	printf("Close socket %d\n",csd);
	delete addr;
	delete cbuffer;
	close(csd);
	pthread_exit(NULL);          /* terminate the thread */
}

void
tcpserver::execute()
{
	list<CBuffer*>::iterator it;
	for(it=m_bufferList.begin();it!=m_bufferList.end();it++)
	{
		if ((*it)->getStatus() > 0 )
		{
			char* buff;
			int n = (*it)->read(buff);
			if (n>0)
			{
				int socket = (*it)->getSocket();
				write(socket,buff,n);
				//printf(">%d\n",n);
				if ((n==6) && (0==strncmp(buff,"quit",4)))
				{
					close(socket);
				}
			}
		}
	}
	
}

void
tcpserver::halt()
{
	m_running = false;
	// TO DO signal to kill all threads
}

