#include "interace.h"

 void getMac(unsigned char & addr){
	int sock,i=0;
	struct ifreq ifr;
	sock=socket(AF_INET,SOCK_STREAM,0);
	strcpy(ifr.ifr_name, "wlan0");
	ioctl(sock,SIOCGIFHWADDR,&ifr);
	addr=(unsigned char *)ifr.ifr_hwaddr.sa_data;

	printf(" HWADDR:");
	for(;i<6;i++){
		printf("%x\n",addr[i]);
	}
	printf("\n");
 }

void getIp(unsigned char & addr){
	int sock,i=0;
	struct ifreq ifr;
	sock=socket(AF_INET,SOCK_STREAM,0);
	strcpy(ifr.ifr_name, "wlan0");
	ioctl(sock,SIOCGIFHWADDR,&ifr);
	addr=(unsigned char *)ifr.ifr_hwaddr.sa_data;

	ioctl(sock,SIOCGIFADDR,&ifr);
	addr=((unsigned char *)ifr.ifr_addr.sa_data)+2;
	printf(" IPADDR:");
	for(;i<4;i++){
		printf("%d\n",addr[i]);
	}
	printf("\n");
 }
