/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#define err_quit(m) { perror(m); exit(-1); }

#define NIPQUAD(s)	((unsigned char *) &s)[0], \
					((unsigned char *) &s)[1], \
					((unsigned char *) &s)[2], \
					((unsigned char *) &s)[3]

static int s = -1;
static struct sockaddr_in sin;
static unsigned seq;
static unsigned count = 0;
const int on = 1;
typedef struct {
	unsigned seq;
	struct timeval tv;
}	ping_t;

double tv2ms(struct timeval* tv) {
	return 1000.0*tv->tv_sec + 0.001*tv->tv_usec;
}
unsigned short cksum(void *in, int sz) {
	long sum = 0;
	unsigned short *ptr = (unsigned short*) in;
	for(; sz > 1; sz -= 2) sum += *ptr++;
	if(sz > 0) sum += *((unsigned char*) ptr);
	while(sum>>16) sum = (sum & 0xffff) + (sum>>16);
	return ~sum;
}
int echoreq1(int s, unsigned short id, unsigned short seq) {
    char buf[64];
    struct ip  *iph = (struct ip  *) buf;
    struct timeval *ptv = (struct timeval *) (buf + sizeof(ip));
	 //10.113.113.255
    bcopy("00000000000000000000", buf,sizeof(ip));
	iph->ip_hl = 5;
	iph->ip_v = 4;
	iph->ip_tos = 0;
	iph->ip_len = sizeof (struct ip )   + sizeof(struct timeval);
	iph->ip_id =0;	//Id of this packet
	iph->ip_off = 0;
	iph->ip_ttl = 255;
	iph->ip_p = 161;
	iph->ip_sum = 0;		//Set to 0 before calculating checksum
	iph->ip_src =   ( sin.sin_addr) ;	//Spoof the source ip address
	iph->ip_dst =  (sin.sin_addr);
	printf(" ip %u.%u.%u.%u send %u.%u.%u.%u\n",NIPQUAD(iph->ip_src),
    NIPQUAD(iph->ip_dst));
    gettimeofday(ptv, NULL);
	// printf("send %lu.%06lu  \n",
    //ptv->tv_sec, ptv->tv_usec);
	iph->ip_sum = cksum ((void *) buf, sizeof (struct ip ) );
	//printf("csum %d\n",iph->check);
    // icmph->icmp_cksum = cksum(buf, 8 + sizeof(struct timeval));
	int sz=write(s, buf, sizeof (struct ip ) + sizeof(struct timeval));
	if (sz<0)	perror("sz");
	printf("write sz %d \n",sz);
   return  sz;
}


void do_send(int sig) {
	unsigned char buf[1024];
	printf("count %d \n",count); 
	if(echoreq1(s, seq, ++seq) < 0) err_quit("echoreq1");
	alarm(1);
	count++;
	if(count > 10) exit(0);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in dst;
	
	if(argc < 2) {

		return -fprintf(stderr, "usage: %s ... <port> <ip>\n", argv[0]);
	}

	srand(time(0) ^ getpid());

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	// sin.sin_port = htons(strtol(argv[argc-2], NULL, 0));
	if(inet_pton(AF_INET, argv[argc-1], &sin.sin_addr) != 1) {
		return -fprintf(stderr, "** cannot convert IPv4 address for %s\n", argv[1]);
	}
	printf("path%s %s %s\n",argv[argc-3] ,argv[argc-2] ,argv[argc-1]);
	
	 
	if((s = socket(AF_INET, SOCK_RAW, 161)) < 0)
		err_quit("socket");
	
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
	if(connect(s, (struct sockaddr*) &sin, sizeof(sin)) < 0) err_quit("c");
	
	
	
	printf("start ping\n");
	signal(SIGALRM, do_send);

	seq = rand() % 0xffffff;
	// fprintf(stderr, "PING %u.%u.%u.%u  , init seq = %d\n",
	// 	NIPQUAD(sin.sin_addr),  seq);

	//do_send(0);
	alarm(1);


	while(1) {
		// int rlen;
		// struct sockaddr_in csin;
		// socklen_t csinlen = sizeof(csin);
		// char buf[2048];
		// struct timeval tv;
		// ping_t *p = (ping_t *) buf;
		
		// if((rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &csin, &csinlen)) < 0) {
		// 	perror("recvfrom");
		// 	continue;
		// }

		// gettimeofday(&tv, NULL);
		// printf("%lu.%06lu %d bytes from %u.%u.%u.%u : seq=%u, time=%.6f ms\n",
		// 		p->tv.tv_sec, p->tv.tv_usec, rlen,
		// 		NIPQUAD(csin.sin_addr) ,
		// 		p->seq, tv2ms(&tv)-tv2ms(&p->tv));
		 
	}

	close(s);
}
