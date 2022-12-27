#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include  <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
 #include <fcntl.h>   
#include <cstring>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#define MAXNAME 1024*33
#define SA struct sockaddr
#define err_quit(m) { perror(m); exit(-1); }
#define NIPQUAD(s)	((unsigned char *) &s)[0], \
					((unsigned char *) &s)[1], \
					((unsigned char *) &s)[2], \
					((unsigned char *) &s)[3]
typedef struct {
	char file_name[6];
	 
}	file_t;
using namespace std;
// Function designed for chat between client and server.
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
    "ip_id=%u ttl=%u \n",
    tv.tv_sec, tv.tv_usec,
    sz,
    NIPQUAD(iph->ip_src),
    NIPQUAD(iph->ip_dst),
     (iph->ip_id),
    iph->ip_ttl ));
 
    return sz;
}

int write_file(string data,string write_in, char file_name[6]){
				
				write_in +="/";
				write_in+= file_name;
				printf("%s\n",write_in.c_str());
				int fd =open(write_in.c_str(), O_WRONLY  |O_TRUNC  );
				
				if(fd<0)return -1;
				 
				 
				if(( write(fd ,data.c_str() ,data.length()))==-1)
						perror("not write");
				 
				// printf("w_buf %ld\n",w_buf);
				close(fd);
			 
} 
 
// Driver function
int main(int argc, char * argv[])
{
	int sockfd, connfd,on=1;
	struct sockaddr_in servaddr, cli;
	string path_store_file=argv[1];
	int total_number_of_files= atoi(argv[2]);

	 
	 



	// socket create and verification
	sockfd = socket(AF_INET, SOCK_RAW,161);
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	
	
	bzero(&servaddr, sizeof(servaddr));

	
	char buf[MAXNAME];
	//struct sockaddr_in myaddr; /* address of this service */
	struct sockaddr_in client_addr; /* address of client    */
	int length = sizeof(client_addr);
	int nbytes;

	
	for ( ; ; ){
				memset(buf, 0, sizeof(buf));

		  		if ((nbytes = recvfrom(sockfd, &buf, MAXNAME, 0, (struct sockaddr*)&client_addr, (socklen_t *)&length)) <0)
					err_quit("recv error :")
				
				
				 
				
                 
                // printf("%s\n", buf);
				



				file_t * fh=(file_t * )buf;
				string data=(buf+sizeof(file_t));
				 
			 
				if(write_file( data,path_store_file,fh->file_name)<0) err_quit("write ");
				
 
                /* return to client */
				 
                

           
	 
		// pid_t childpid;
		// if ( (childpid = fork()) == 0) { /* child process */
		// 	print(childpid)

		// 	exit(0);
		// }
		 
	}
	
		
	
	// Function for chatting between client and server
	//func(connfd);

	// After chatting close the socket

	close(sockfd);
}
