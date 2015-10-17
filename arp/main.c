#include <stdio.h>
#include <stdlib.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in.h>
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
 u_char d_mac_addr[HWLEN];   // fff....
 u_char s_mac_addr[HWLEN];  //1getmac()
 u_short type;     //0x0806
} _ethHeader;

typedef struct{
 u_short htype;  //0x0001
 u_short ptype;      // 0x0800
 u_char hw_len;     //6
 u_char p_len;       // 4
 u_short oper;     // req 1
 u_char sen_hw_addr[HWLEN];  // getMac()
 u_char sen_p_addr[4];    //getIp();
 u_char dst_hw_addr[HWLEN];   // 00...
 u_char dst_p_addr[4];    // ip
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
 if(argc <1){
	printf("need IP\n");
	exit(1);
	}

 int sock;
 struct sockaddr_ll *addr,*recvAddr;
 arpPacket *packet,*recvPacket;

 addr=malloc(sizeof(struct sockaddr_ll));
 recvAddr=malloc(sizeof(struct sockaddr_ll));
 packet=malloc(sizeof(arpPacket));
 recvPacket=malloc(sizeof(arpPacket));
 memset(recvPacket,0,sizeof(arpPacket));

 unsigned char dstIP[]={192,168,1,1};
 unsigned char srcIP[]={192,168,1,244};
 unsigned char dstMac[]={255,255,255,255,255,255};
 unsigned char srcMac[]={0x9c,0x4e,0x36,0x99,0x7c,0x10};

 headerSet(packet,srcMac,srcIP,dstMac,dstIP); 

 sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ARP));
 if(sock<0)
	{ printf("error sock\n");}
 setDst(addr);


 if(sendto(sock,packet,sizeof(arpPacket),0,(struct sockaddr*)addr,sizeof(struct sockaddr_ll))<0) {
	printf("send sock error\n");
	}
 int request=htons(OPREP);
 int rcv;
 size_t rcvAddrlen=sizeof(struct sockaddr_ll);
 while(1)
 {
	memset(recvAddr,0,sizeof(struct sockaddr_ll));
	rcv=recvfrom(sock,recvPacket,sizeof(arpPacket),0,(void *)recvAddr,(socklen_t*)&rcvAddrlen);
	if(rcv<0) 
		{ printf("%d\n",rcv);}
	if(memcmp(&recvPacket->arpHeader.oper,&request,2)==0){
		break;
	}
 }
 recvPrint(recvPacket);
 
return 0;
}
