#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char message[200], prev_msg[200];
	char addr[20] = "127.0.0.1";
	unsigned short int port = 8888;

	memset(prev_msg, '\0', 200 );

	if( argc>1 ){
		strcpy( addr, argv[1] );
	}

	if( argc>2 ){
		port = (unsigned short int)atoi( argv[2] );
	}
	
	server.sin_addr.s_addr = inet_addr( argv[1] );
	server.sin_family = AF_INET;
	server.sin_port = htons( port );

	//keep communicating with server
	while(1)
	{
		printf("Enter message : ");
		char* ret = fgets (message , 2000 , stdin);
		if( ret == NULL ){
			printf( "Failed to get input from stdin.\n");
			break;
		}
		message[strlen(message)-1]='\0';

		if( strlen(message) == 0 ){
			strcpy( message, prev_msg );	
		}
		
		//Create socket
		sock = socket(AF_INET , SOCK_STREAM , 0);
		if (sock == -1)
		{
			printf("Could not create socket");
			return -1;
		}
		
		//Connect to remote server
		if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
		{
			perror("connect failed. Error");
			return 1;
		}
		
		strcpy( prev_msg, message );
		//Send some data
		if( send(sock , message , 200, 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		
		size_t read_size;
		int bytes_read = 0;
		char incoming_msg[2000];
		char temp[2000];
		while( bytes_read < 2000 ){
			read_size = recv(sock , temp, 2000, 0);
			if(read_size <= 0){
			    puts("Server disconnected");
			    fflush(stdout);
			    close(sock);
			    return 0;
			}
			memcpy( incoming_msg+bytes_read, temp, read_size );
			bytes_read += read_size;
		}

		printf( "Server response: %s\n", incoming_msg );
		close(sock);
	}
	
	return 0;
}

