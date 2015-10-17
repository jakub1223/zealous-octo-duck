#include <stdio.h>
#include <stdlib.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <net/ethernet.h>

#include "error.h"
#include "interace.h"

#define ETHTYPE 0x0806
#define HTYPE 0x0001
#define PTYPE 0x0800
#define HWLEN 6
#define PLEN 4
#define OPREQ  1
#define OPREP 2

typedef struct{
 unsigned char d_mac_addr[6];   // fff....
 unsigned char s_mac_addr[6];  //1getmac()
 unsigned char type[2];     //0x0806
} _ethHeader;

typedef struct{
 unsigned char htype[2];  //0x0001
 unsigned char ptype;      // 0x0800
 unsigned char hw_len;     //6
 unsigned char p_len;       // 4
 unsigned char oper[2];     // req 1
 unsigned char sen_hw_addr[6];  // getMac()
 unsigned char sen_p_addr[4];    //getIp();
 unsigned char dst_hw_addr[6];   // 00...
 unsigned char dst_p_addr[4];    // ip
} _arpHeader;

typedef struct{
 _ethHeader ethHeader;
 _arpHeader arpHeader;
} arpPacket;

void headerSet(arpPacket &Packet, unsigned char &dstIP){
 memset(&Packet,0,sizeof(arpPacket));
 memset(&Packet.ethHeader.d_mac_addr,0xff,6);
 getMac(Packet.ethHeader.s_mac_addr);
 memcpy(Packet.ethHeader.type, htons(ETHTYPE) , HWLEN):
 memcpy(Packet.arpHeader.htype, htons(HTYPE), 2) ;
 Packet.arpHeader.hw_len=HWLEN;
 Packet.arpHeader.p_len=PLEN;
 memcpy(Packet.arpHeader.oper,htons(OPREQ),2);
 getMac(Packet.arpHeader.sen_hw_addr);
 getIp(Packet.arpHeader.sen_ip_addr);
 memset(Packet.arpHeader.dst_hw_addr,0,HWLEN);
 memcpy(Packet.arpHeader.dst_p_addr,dstIp,4);
}

void setDst(struct sockaddr_ll &addr){
 memset(addr,0,sizeof(struct sockaddr_ll));
 addr.sll_family=AF_PACKET;
 addr.sll_ifindex=getIndex();
 addr.sll_halen=HWLEN;
}

int main(int argc, char *argv[]){
 if(argc <1){
	printf("need IP\n");
	exit(1);
	}

 int sock;
 struct sockaddr_ll addr,recvAddr;
 arpPacket packet,recvPacket;
 memset(recvPacket,0,sizeof(arpPacket));

 headerSet(&packet,inet_aton(argv[1])); 

 sock=socket(PF_PACKET,SOCKET_RAW,htons(ETH_P_ARP);
 setDst(&addr);
 
 sendto(sock,&packet,sizeof(arpPacket),0,(struct sockaddr*)addr,sizeof(struct sockaddr));

 while(1){
	memset(recvAddr,0,sizeof(struct sockaddr_ll));
	recvfrom(sock,&recvPacket,sizeof(arpPacket),0,(struct sockaddr*)recvAddr,sizeof(struct sockaddr));
	if(memcmp(recvPacket.arpHeader.oper,(void *)htons(OPREP),2)!=0){
		break;
	}
 }
 
return 0;
}
