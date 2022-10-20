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
#define MAX 80
#define PORT 11111
#define SA struct sockaddr

// Function designed for chat between client and server.
void func(int connfd)
{
	char buff[MAX];
	int n;
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("From client: %s\t To client : ", buff);
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n');

		// and send that buffer to client
		write(connfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

void prob(int connfd,char* argv[]){
	
			dup2( connfd, STDOUT_FILENO );
			if (sizeof(argv[2])/sizeof(char)<6){
				execv(argv[2],argv+2);
				perror("execv  "   );
			}
			else{
				if(execvp(argv[2],argv+2)==-1)
				{
					///dup2( connfd, STDERR_FILENO );
					perror("execvp" );	
				}
				 
				
			}
			


}
void prob4(int connfd,char* argv[]){
			printf("prob4 \n");
			dup2( connfd, STDOUT_FILENO );
			dup2( connfd, STDERR_FILENO );
			
			execvp(argv[2],argv+2) ;
			 
			//perror("execvp" );
			 
			


}
void prob5(int connfd,char* argv[]){
			printf("prob4 \n");
			dup2( connfd, STDOUT_FILENO );
			dup2( connfd, STDERR_FILENO );
			dup2( connfd, STDIN_FILENO );
			
			execvp(argv[2],argv+2) ;
			 
			//perror("execvp" );
			 
			


}

void sig_chld(int signo)
{
pid_t pid;
int stat;
while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
printf("child %d terminated\n", pid);
return;
}
// Driver function
int main(int argc, char * argv[])
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
	signal(SIGCHLD,sig_chld);
	signal(SIGTERM,sig_chld);
	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
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
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		perror("bind");
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
    uint32_t len = sizeof(cli);

	// Accept the data packet from client and verification
 

	for ( ; ; ){
	connfd = accept(sockfd, (SA*)&cli, &len);
	sockaddr_in get_ip_port;
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else{printf("server accept the client...\n");
		if (getpeername(connfd, (struct sockaddr *)&get_ip_port, &len) == -1)
    		perror("getsockname");
		else{
			printf("port number %s %u\n", inet_ntoa(get_ip_port.sin_addr) ,ntohs(get_ip_port.sin_port));
			 
		}
		
			//printf("port number %s %u\n", inet_ntoa(cli.sin_addr) ,ntohs(cli.sin_port));
		pid_t childpid;
		if ( (childpid = fork()) == 0) { /* child process */
			close(sockfd); /* close listening socket */
			if( argc==3)
				prob(connfd,argv);
			else if (sizeof(argv[2])/sizeof(char)==3)
				prob4(connfd,argv);
			else if (sizeof(argv[2])/sizeof(char)==8)
				prob5(connfd,argv);

			exit(0);
		}
		close(connfd);	
	}

	}
		
	
	// Function for chatting between client and server
	//func(connfd);

	// After chatting close the socket

	close(sockfd);
}
