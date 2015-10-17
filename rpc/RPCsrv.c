#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define PORTSRV 6004
#define PORTCLI 6003
#define MAXCLIENTS 2

int connectedClients=0;

void killchld(int sign)
{
 int pid=wait(NULL);
 connectedClients--;  //decrease number of clients
}


int main(int *argc, char **argv)
{
 signal(SIGCHLD,killchld);
 printf("%d  \n",getpid());
 char buf[1024];
 char comm1[64], *tmp;
 int sock, acc,pid;

 struct sockaddr_in  srv;
 struct sockaddr_in cli[MAXCLIENTS];

 bzero((char *) &cli, sizeof(cli));
 bzero((char *) &srv, sizeof(srv));

 srv.sin_family=AF_INET;
 srv.sin_port=htons(PORTSRV);
 srv.sin_addr.s_addr=INADDR_ANY;

 sock=socket(AF_INET,SOCK_STREAM,0);
 if(sock<0) { printf("sock"); exit (1);}

 if(bind(sock, (struct sockaddr*)&srv,sizeof(srv))<0) {printf("bind"); exit(1);}
 listen(sock,1);

while(1){               //main srv loop
 while(connectedClients<2)		//max. numb of clients
 {	

	printf("accepting\n");
	acc=accept(sock,NULL, NULL);
	if(acc<0) { printf("%d",acc); exit(1);}
	printf("new connection established\n");
	connectedClients++; 

	pid=fork();
	
  if(pid==0)
	{
		dup2(acc,1);
		while(1)
		{ 
			memset(comm1,0,63);
			int rcv=recv(acc,comm1,sizeof(comm1),0);
			if(rcv==1){
				break;
				}
			tmp=malloc(rcv);     
			strncpy(tmp,comm1,rcv);	
			system(tmp);           
		}
		fwrite("connection closed\n",17,1, stderr);
		close(acc);
		_exit(0);
	}
	close(acc);
 }
 sleep(1);
 printf("waiting\n");

}
close(sock);

 return 0;
}
 
