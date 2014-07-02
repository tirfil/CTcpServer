#include <stdio.h>
#include "tcpserver.h"

int main(int argc, char **argv)
{
	tcpserver* server = new tcpserver(2000);
	server->init();
	printf("Finished\n");
	while(true) {
		server->execute();
	}
	return 0;
}
