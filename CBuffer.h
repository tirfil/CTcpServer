#ifndef CBUFFER_H
#define CBUFFER_H

#include <string.h>

#include <queue>
using namespace std;


struct Data
{
	char* data;
	int len;
};

class CBuffer
{
private:
	int m_rxflag;
	int m_txflag;
	queue<Data*> m_buffer;
	char* m_address;
	int m_status;
	int m_socket;
public:
	CBuffer();
	//~CBuffer();
	int write(char* data, int len);
	int read(char* &data);
	void clear();
	char* getAddress();
	void setAddress(char* addr);
	int getStatus();
	void setStatus(int status);
	int getSocket();
	void setSocket(int sock);
};

#endif