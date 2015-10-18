#include "interace.h"

#include <stdio.h>
#include <stdlib.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h> 

 void getMac(unsigned char * addr){
	int sock,i=0;
	struct ifreq ifr;
	sock=socket(AF_INET,SOCK_STREAM,0);
	strcpy(ifr.ifr_name, "wlan0");
	ioctl(sock,SIOCGIFHWADDR,&ifr);
	memcpy(addr,&ifr.ifr_hwaddr.sa_data,6);
	printf(" HWADDR:");
	for(;i<6;i++){
		printf("%x:",addr[i]);
	}
	printf("\n");
	close(sock);
 }

void getIP(unsigned char * addr){
	int sock,i=0;
	struct ifreq ifr;
	sock=socket(AF_INET,SOCK_STREAM,0);
	strcpy(ifr.ifr_name, "wlan0");
	ioctl(sock,SIOCGIFADDR,&ifr);
	addr=((unsigned char *)ifr.ifr_addr.sa_data)+2;
	printf(" IPADDR:");
	for(;i<4;i++){
		printf("%d.",addr[i]);
	}
	printf("\n");
	close(sock);
 }

 int getIndex(){
	int sock,i=0;
	struct ifreq ifr;
	sock=socket(AF_INET,SOCK_STREAM,0);
	strcpy(ifr.ifr_name, "wlan0");
	ioctl(sock,SIOCGIFINDEX,&ifr);
 return ifr.ifr_ifindex;
 }


