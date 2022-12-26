/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#define err_quit(m) { perror(m); exit(-1); }
#define NIPQUAD(s)	((unsigned char *) &s)[0], \
					((unsigned char *) &s)[1], \
					((unsigned char *) &s)[2], \
					((unsigned char *) &s)[3]
double tv2ms(struct timeval tv) {
    return 1000.0 * tv.tv_sec + 0.001 * tv.tv_usec; }
					
int echorsp1(int s, unsigned short id, struct sockaddr_in src) {
    int sz;
    char buf[2048];
    struct ip  *iph = (struct ip  *) buf;
     
    struct timeval tv, *ptv = NULL;
 
     if((sz = recv(s, buf, sizeof(buf), MSG_TRUNC)) <= 0) return sz;
 
 
	
	gettimeofday(&tv, NULL);
	if(iph->ip_v != 4) return -1;
	// if(iph->ip_src.s_addr != src.sin_addr.s_addr) return -1;
	// if(htons(iph->ip_len) < (iph->ip_hl<<1) + 8) return -1
 
    ptv = (struct timeval *) (buf + (iph->ip_hl<<2) );
 
    printf("%lu.%06lu: %d bytes from %u.%u.%u.%u to %u.%u.%u.%u: "
    "icmp_seq=%u ttl=%u time=%.4f ms\n",
    tv.tv_sec, tv.tv_usec,
    sz,
    NIPQUAD(iph->ip_src),
    NIPQUAD(iph->ip_dst),
     (iph->ip_id),
    iph->ip_ttl,
    tv2ms(tv) - tv2ms(*ptv));
 
    return sz;
}
int main(int argc, char *argv[]) {
	const int on = 1;	
	int s;
	struct sockaddr_in sin;

	if(argc < 2) {
		return -fprintf(stderr, "usage: %s ... <port>\n", argv[0]);
	}

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&sin, 0, sizeof(sin));


	printf("server br %s \n",argv[argc-1]);
	// sin.sin_family = AF_INET;
	// sin.sin_port = htons(strtol(argv[argc-1], NULL, 0));

	if((s = socket(AF_INET,SOCK_RAW,161)) < 0)
		err_quit("socket RAW");
	
	
	 
	
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));

	// if(bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0)
	// 	err_quit("bind");
	
	
	

	while(1) {
	
		if(echorsp1(s, 1, sin) <= 0) break;
	}

	close(s);
}
