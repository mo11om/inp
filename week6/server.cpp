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
#include <time.h>
#include <ctime>
#include <iostream>
#include <cerrno>
#include <poll.h>
#include <errno.h>
#include <stdarg.h>
 
#include "aa.h"
#define MAXLINE 80
#define Animal_Num 889
#define adj_Num 3434
#define SA struct sockaddr
#define OPEN_MAX 1024
#define INFTIM -1



void hello_client(int connfd)
{
	//char buff[MAX];

	uint32_t len=sizeof(SA);
    sockaddr_in get_ip_port;
    if (getpeername(connfd, (struct sockaddr *)&get_ip_port, &len) == -1)
    		perror("getsockname");
		else{
			printf("port number %s %u\n", inet_ntoa(get_ip_port.sin_addr) ,ntohs(get_ip_port.sin_port));
			 
		}
    srand ( time(NULL) );
	time_t now = time(0);
   
   // convert now to string form
   	struct tm *Time_now = localtime(&now);

    int AniIndex=  random ()%Animal_Num;
    int adjIndex = random ()%adj_Num;
     
    // char * execvp_str[] = {"date",NULL};
    // execvp("date",execvp_str);
	
	// write(connfd,& adjs[adjIndex],sizeof(int));write(connfd,& animals[AniIndex],sizeof(int));
	 char buffer[32];
    
	sprintf(buffer,"%i-%i-%i %i:%i:%i *** %s %s \n",Time_now->tm_year+1900,Time_now->tm_mon ,Time_now->tm_mday,Time_now->tm_hour,Time_now->tm_min ,Time_now->tm_sec ,adjs[adjIndex] , animals[AniIndex]);
    printf("%s \n"  ,buffer);
	write(connfd, buffer,32);
	// infinite loop for chat
	// for (;;) {
        
		
	// }
}

 

// Driver function
int main(int argc, char * argv[])
{
	int i, maxi, listenfd, connfd, sockfd;
	int nready;
	ssize_t n;
	char buf[MAXLINE];
	socklen_t clilen;
	struct pollfd client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;

	// signal(SIGCHLD,sig_chld);
	// signal(SIGTERM,sig_chld);
	// socket create and verification
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, 
	uint32_t port =std::atoi(argv[1]) ;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification
	if ((bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		perror("bind");
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(listenfd, 256)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
    
	 
	// Accept the data packet from client and verification
	
	
	 
	
	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for (i = 1; i < OPEN_MAX; i++)
		client[i].fd = -1; /* -1: available entry */
	maxi = 0; /* max index in client[] */

	
	for ( ; ; ){
		
	 	 
		     
		
		nready = poll(client, maxi+1, INFTIM);
		
		if (client[0].revents & POLLRDNORM) { /* new client */
			printf("server accept the client...\n");
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
			hello_client (connfd);
			for (i = 1; i < OPEN_MAX; i++)
				if (client[i].fd < 0) {
				client[i].fd = connfd; /* save descriptor */
				break;
				}
			if (i == OPEN_MAX) printf("too many clients");//err_quit("too many clients");
			
			
			 
			client[i].events = POLLRDNORM;
			
			if (i > maxi) maxi = i; /* max index in client[] */
			if (--nready <= 0) continue; /* no more readable descs */
		}
		printf("start checkinng \n");
		for (i = 1; i <= maxi; i++) {
			/* check all clients for data */
			if ( (sockfd = client[i].fd) < 0) continue;
			if (client[i].revents & (POLLRDNORM | POLL_ERR)) {
				if ( (n = read(sockfd, buf, MAXLINE)) < 0) {
					if (errno == ECONNRESET) {
						/* connection reset by client */
						close(sockfd);
						client[i].fd = -1;
						} 
					else printf("read error");
				} 
				else if (n == 0) { /* connection closed by client */
					printf("client  %i shutdown \n" ,sockfd );

					close(sockfd);
					client[i].fd = -1;
				}
				else{
					//printf("%s \n",buf);
					write(sockfd, buf, n);
				}
					
				if (--nready <= 0) break; /* no more readable descs */
			}
		}

		// 	
	 

	}
		
	
	// Function for chatting between client and server
	//func(connfd);

	// After chatting close the socket

	close(sockfd);
}