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
		char buf[MAX];

		unsigned char *name ;unsigned short qtype;
		
	
		int rlen;
		if((rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &csin, &csinlen)) < 0) {
			perror("recvfrom");
			break;
		}
		
		//fix question
		unsigned char* qname = (unsigned char*) &buf[sizeof(struct DNS_HEADER)];
 		struct QUESTION* qinfo = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER)
			+ (strlen((const char*) qname) + 1)];
		//get name and type
		name= get_qname( buf  , qname );qtype=ntohs(qinfo->qtype);
		printf("name %s \n",name);
		//find 

 		send_dns(s,buf,csin);
		//foreign(s,buf,rlen,csin)	; 
		
	
	 
		
		//sendto(s, buf, rlen, 0, (struct sockaddr*) &csin, sizeof(csin));
		 

	}

	close(s);
}
