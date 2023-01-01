/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "dns_.h"
// #include "self.h"
#define err_quit(m) { perror(m); exit(-1); }
#define MAX 65536
int main(int argc, char *argv[]) {
	int s;
	struct sockaddr_in sin;

	if(argc < 2) {
		return -fprintf(stderr, "usage: %s ... <port>\n", argv[0]);
	}

 

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(strtol(argv[argc-2], NULL, 0));
	
	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");

	if(bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0)
		err_quit("bind");

	while(1) {
		struct sockaddr_in csin;
		socklen_t csinlen = sizeof(csin);
		unsigned char  *qname, *reader;
		char buf[MAX];
		struct  DNS_HEADER *dns = NULL;
		struct QUESTION *qinfo = NULL;
		struct RES_RECORD *ans =new RES_RECORD;
		char root[23]; 
		int rlen;
		
		if((rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &csin, &csinlen)) < 0) {
			perror("recvfrom");
			break;
		}
		
		//foreign(s,buf,rlen,csin)	; 
		 send_dns(s,buf,csin);
	
	// 	// reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*) qname) + 1)
	// 	// 	+ sizeof(struct QUESTION)];
	// 	qname = (unsigned char*) &buf[sizeof(struct DNS_HEADER)];
	// 	qinfo = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER)
	// 		+ (strlen((const char*) qname) + 1)];
	// 	ans=(struct RES_RECORD*) &buf[sizeof(struct DNS_HEADER)
	// 		+ (strlen((const char*) qname) + 1)
	// 		+ sizeof( struct QUESTION)];
	// 	memcpy(&buf[sizeof(struct DNS_HEADER)
	// 		+ (strlen((const char*) qname) + 1)
	// 		+ sizeof(struct QUESTION)], 
	// 		root,23
	// 		);
			
	// 	dns->qr=1;
	// 	dns->aa=1;
	// 	dns->ra=1;
	// 	dns->ad=0;
	// 	dns->rcode=0;
	// 	printf("\nThe response contains : ");
	// printf("\n %d Questions.", ntohs(dns->q_count));
	// printf("\n %d Answers.", ntohs(dns->ans_count));
	// printf("\n %d Authoritative Servers.", ntohs(dns->auth_count));
	// printf("\n %d Additional records. ", ntohs(dns->add_count));
	// printf("\n qname %s  length %ld  ", qname,strlen( (const char*)qname));
	// printf("\n qtype %d  ",ntohs( qinfo->qtype));
	// printf("\n qclass %d \n\n", ntohs(qinfo->qclass));
		 
		//  own_dns(qname);
		
		//sendto(s, buf, rlen, 0, (struct sockaddr*) &csin, sizeof(csin));
		 

	}

	close(s);
}
