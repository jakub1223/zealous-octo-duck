#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#define BUFFSIZE 256

void threadRcv(int sock){
while(1){
 struct sockaddr_in ad;
 char buffRcv[BUFFSIZE];
 bzero(buffRcv,BUFFSIZE);
 recvfrom(sock, buffRcv, BUFFSIZE, 0,(struct sockaddr*)&ad,sizeof(ad));
 printf(" %s\n",buffRcv);
 }
}

int main(int argc,char **argv){
 int sock,nickLen;
 char buffSnd[BUFFSIZE], nick[10];
 struct sockaddr_in addr;
 struct ip_mreqn mreq;
 pthread_t thread;

 bzero(nick,10);
 scanf("%s",nick);

 sock=socket(AF_INET,SOCK_DGRAM,0);
 if(sock<0){
  printf("socket %s \n",strerror(errno));
  exit(1);
  }

 bzero(&addr,sizeof(struct sockaddr_in));
 addr.sin_family=AF_INET;
 addr.sin_port=htons(12345);
 addr.sin_addr.s_addr =inet_addr("224.0.0.3");

 if(bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))<0){
  printf("bind %s \n",strerror(errno));
  close(sock);
  exit(1);
 }

 mreq.imr_multiaddr.s_addr=inet_addr("224.0.0.3");
 mreq.imr_address.s_addr=inet_addr("192.168.1.12");
 mreq.imr_ifindex=0;

 if(setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0){
        printf(" %s \n",strerror(errno));
        close(sock);
        exit(1);
        }

 pthread_create(&thread,NULL,threadRcv,(void *)sock);

 int msgSize;
 char msgBuff[BUFFSIZE-10]="hello";

 do{
	bzero(buffSnd,BUFFSIZE);
	msgSize=sprintf(buffSnd,"%s: %s",nick,msgBuff);
	if(sendto(sock,buffSnd,msgSize,0,(struct sockaddr*)&addr,sizeof(addr))<0){
	printf("send %s \n",strerror(errno));
	close(socket);
	exit(1);
	}
	bzero(msgBuff,BUFFSIZE-10);
	scanf("%s",msgBuff);
 }while(memcmp(msgBuff,"quit",4));

 close(sock);
return 0;
}
