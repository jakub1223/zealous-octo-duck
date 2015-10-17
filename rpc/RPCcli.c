#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <time.h>
#include <string.h>

#define PORTSRV 6004
#define PORTCLI 6003
int main(int *argc, char **argv)
{
 char buf[1024];
 
 char command[64];

 int sock;
 struct sockaddr_in  srv,cli;
 struct timespec time1, time2;

 bzero((char *) &srv, sizeof(cli));
 bzero((char *) &srv, sizeof(srv)); 

 srv.sin_family=AF_INET;
 srv.sin_port=htons(PORTSRV);
 srv.sin_addr.s_addr=inet_addr("127.0.0.1");

 cli.sin_family=AF_INET;
 cli.sin_port=htons(PORTCLI);
 cli.sin_addr.s_addr=INADDR_ANY;
 
 
 sock=socket(AF_INET,SOCK_STREAM,0);
 if(sock<0) { printf("sock"); exit(1); }
 
 if(bind(sock, (struct sockaddr*)&cli,sizeof(cli))       <0){
 	 printf("bind"); exit(1); }

 if(connect(sock, (struct sockaddr*)&srv,sizeof(srv))      <0) { 
 	printf("connect"); exit(1);}

 do{
	memset(buf,0,1024);
	memset(command,0,64);
	fgets(command, 64, stdin);
	printf("snd\n");
	if(send(sock,command,strlen(command),0) <0){printf("send"); exit(1);}
	printf("rcv\n");
	//if(!strcmp("quit",command)){break; }
	if(recv(sock,&buf,sizeof(buf),0) <0){printf("recv"); exit(1);}
	printf("buffer: %s ",buf);
 } while(1);
 close(socket);
 return 0;
}

