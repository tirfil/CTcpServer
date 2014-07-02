#include "CBuffer.h"

#include <stdio.h>
#include <stdlib.h>


CBuffer::CBuffer()
{
	m_rxflag = 0;
	m_txflag = 0;
	m_socket = 0;
	
	m_address = NULL;
	clear();
}

void
CBuffer::clear()
{
	while(!m_buffer.empty())
	{
		m_buffer.pop();
	}
	m_status = 0;
}

int
CBuffer::write(char* data, int len)
{
	m_txflag = 1;
	if (m_rxflag == 0)
	{
		char* buffer = new char(len);
		//printf("0x%x\n",buffer);
		strncpy(buffer,data,len);
		Data* info = new Data();
		info->data = buffer;
		info->len = len;
		m_buffer.push(info);
		m_txflag = 0;
		m_status += 1;
		return len;
	} 
	else
	{
		m_txflag = 0;
		return -1;
	}
}

int
CBuffer::read(char* &data)
{
	int len;
	Data* info;
	m_rxflag = 1;
	if (m_txflag == 0)
	{
		if (m_buffer.empty())
			return 0;
		info = m_buffer.front();
		//printf(" %s\n",info->data);
		m_buffer.pop();
		data = info->data;
		len = info->len;
		delete info;
		m_rxflag = 0;
		m_status -= 1;
		return len;
	}
	else
	{
		m_rxflag = 0;
		return -1;
	}
}

char*
CBuffer::getAddress()
{
	return m_address;
}

void
CBuffer::setAddress(char* addr)
{
	m_address = addr;
}

int
CBuffer::getStatus()
{
	return m_status;
}

void
CBuffer::setStatus(int status)
{
	m_status = status;
}

int
CBuffer::getSocket()
{
	return m_socket;
}

void 
CBuffer::setSocket(int sock)
{
	m_socket = sock;
}
