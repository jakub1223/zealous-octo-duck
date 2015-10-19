#include <stdio.h>
#include <stdlib.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> 

#include "error.h"
#include "interace.h"

#define ETHTYPE 0x0806
#define HTYPE 0x0001
#define PTYPE 0x0800
#define HWLEN 6
#define PLEN 4
#define OPREQ 1
#define OPREP 2

typedef struct{
 u_char d_mac_addr[HWLEN];   
 u_char s_mac_addr[HWLEN]; 
 u_short type;  
} _ethHeader;

typedef struct{
 u_short htype; 
 u_short ptype;    
 u_char hw_len;   
 u_char p_len;    
 u_short oper;     
 u_char sen_hw_addr[HWLEN];  
 u_char sen_p_addr[4];   
 u_char dst_hw_addr[HWLEN];   
 u_char dst_p_addr[4];  
} _arpHeader;

typedef struct{
 _ethHeader ethHeader;
 _arpHeader arpHeader;
} arpPacket;

void headerSet(arpPacket * Packet, 
				unsigned char *srcMac,
				unsigned char *srcIP,
				unsigned char *dstMac,
				unsigned char *dstIP
				)
{
 memset(Packet,0x00,sizeof(arpPacket));
 memcpy(&Packet->ethHeader.d_mac_addr,dstMac,HWLEN);
 memcpy(&Packet->ethHeader.s_mac_addr,srcMac,HWLEN);
 Packet->ethHeader.type=htons(ETHTYPE);
 Packet->arpHeader.htype=htons(HTYPE);
 Packet->arpHeader.ptype=htons(PTYPE);
 Packet->arpHeader.hw_len=HWLEN;
 Packet->arpHeader.p_len=PLEN;
 Packet->arpHeader.oper=htons(OPREQ);
 memcpy(&Packet->arpHeader.sen_hw_addr,srcMac,6);
 memcpy(&Packet->arpHeader.sen_p_addr,srcIP,4);
 memset(&Packet->arpHeader.dst_hw_addr,0,HWLEN);
 memcpy(&Packet->arpHeader.dst_p_addr,dstIP,4);
}

void setDst(struct sockaddr_ll * addr)
{
 memset(addr,0,sizeof(struct sockaddr_ll));
 addr->sll_family=AF_PACKET;
 addr->sll_ifindex=getIndex();
 addr->sll_halen=HWLEN;
}

void recvPrint(arpPacket * packet)
{
 int i=0;
 for(;i<6;i++){
	printf("%02X:",packet->arpHeader.sen_hw_addr[i]);
 }
 printf("\n");
}

int main(int argc, char *argv[]){
 if(argc < 1){
	printf("need IP\n");
	exit(1);
	}

 int sock;
 struct sockaddr_ll *addr,*recvAddr;
 arpPacket *packet,*recvPacket;

 sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ARP));
 if(sock<0){ 
	printf("error sock\n");
	exit(1);
	}

 addr=malloc(sizeof(struct sockaddr_ll));
 recvAddr=malloc(sizeof(struct sockaddr_ll));
 packet=malloc(sizeof(arpPacket));
 recvPacket=malloc(sizeof(arpPacket));
 memset(recvPacket,0,sizeof(arpPacket));

 unsigned char dstMac[]={255,255,255,255,255,255};
 unsigned char *srcIP,*srcMac;
 unsigned char *dstIP;

 dstIP=malloc(PLEN);
 sscanf(argv[1], "%d.%d.%d.%d", dstIP, dstIP+1, dstIP+2, dstIP+3);

 srcMac=malloc(HWLEN);
 getMac(srcMac);

 srcIP=malloc(PLEN);
 getIP(srcIP);
 
 headerSet(packet,srcMac,srcIP,dstMac,dstIP); 

 setDst(addr);

 if(sendto(sock,packet,sizeof(arpPacket),0,(struct sockaddr*)addr,sizeof(struct sockaddr_ll))<0) {
	printf("send  error\n");
	close(socket);
	}


 int request=htons(OPREP);
 int rcv;
 size_t rcvAddrlen=sizeof(struct sockaddr_ll);

 memset(recvAddr,0,sizeof(struct sockaddr_ll));
 rcv=recvfrom(sock,recvPacket,sizeof(arpPacket),0,(void *)recvAddr,(socklen_t*)&rcvAddrlen);
 if(rcv<0) 
	{ printf("%d\n",rcv);}


 recvPrint(recvPacket);
 


 free(addr);
 free(recvAddr);
 free(packet);
 free(recvPacket);
 free(srcIP);
 free(dstIP);
 free(srcMac);
return 0;
}
