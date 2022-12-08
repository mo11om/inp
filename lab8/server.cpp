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
#define MAXNAME 1024*33
#define SA struct sockaddr
using namespace std;
// Function designed for chat between client and server.
 

 
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
	string path_store_file=argv[1];
	int total_number_of_files= atoi(argv[2]);

	uint32_t port =std::atoi(argv[3]) ;
	signal(SIGCHLD,sig_chld);
	signal(SIGTERM,sig_chld);



	// socket create and verification
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, 
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		perror("bind");
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	// if ((listen(sockfd, 5)) != 0) {
	// 	printf("Listen failed...\n");
	// 	exit(0);
	// }
	// else
	// 	printf("Server listening..\n");
    // uint32_t len = sizeof(cli);
 
	// Accept the data packet from client and verification
	char buf[MAXNAME];
	struct sockaddr_in myaddr; /* address of this service */
	struct sockaddr_in client_addr; /* address of client    */
	int length = sizeof(client_addr);
	int nbytes;

	
	for ( ; ; ){
				memset(buf, 0, sizeof(buf));

		  		if ((nbytes = recvfrom(sockfd, &buf, MAXNAME, 0, (struct sockaddr*)&client_addr, (socklen_t *)&length)) <0) {
                        perror ("could not read datagram!!");
                        continue;
                }
				
				 
				
                printf("Received data form %s : %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
                // printf("%s\n", buf);
				
				string read_data=buf;int data_size=read_data.length()-6-5;
				 
				string file_name=read_data.substr(0,6);
				string	real_date_size=read_data.substr(6,6);
				string data=read_data.substr(12);
				
				cout<<file_name<<" "<<real_date_size<<" "<<data.length()<<"data";
				int cmp_size =stoi(real_date_size);
				if (cmp_size!=data.length())continue; 
				


				string tmp = argv[1];
				tmp +="/";
				tmp+= file_name;
				printf("%s\n",tmp.c_str());
				int fd =open(tmp.c_str(), O_WRONLY  |O_TRUNC  );
				if(fd==-1){
					perror("open not exist");
					fd =creat(tmp.c_str(), 0777) ;
					 
					
				}
				if(fd==-1)
					perror("not open");
				 
				 
				if(( write(fd ,data.c_str() ,data.length()))==-1)
						perror("not write");
				 
				// printf("w_buf %ld\n",w_buf);
				close(fd);
			 

				string rec_msg= "receive"+ to_string(data.length());
				cout<<"send"<<rec_msg<<endl;

                /* return to client */
				for (int i= 0;i<5; i++){
					if (sendto(sockfd, rec_msg.c_str(), rec_msg.length(), 0, (struct sockaddr*)&client_addr, length) < 0) {
							perror("Could not send datagram!!\n");
							continue;
					}

				}
                

           
	 
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
