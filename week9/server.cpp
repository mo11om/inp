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
#include<sys/time.h>
#include <sstream>
#include <vector>
#include <map>
 
#define MAXLINE 500
#define Animal_Num 889
#define adj_Num 3434
#define SA struct sockaddr
#define OPEN_MAX 1024
#define INFTIM -1
using namespace std;



map<int, string[2]> map_Name_Ip_and_port;
map<int, string[2]> map_sink_Name_Ip_and_port;
long long counter=0;
double reset_time=0.0;
double tv2s(struct timeval *ptv) {
	return 1.0*(ptv->tv_sec) + 0.000001*(ptv->tv_usec);
}

string date_get(  ){
	string date_str;
    char buffer[32];
	
	timeval now;
	// time_t now = time(NULL);
	gettimeofday(&now,NULL); 

   // convert now to string form
   	// struct tm *Time_now = localtime(&now);
	
	// sprintf(buffer,"%i-%i-%i %i:%i:%i *** ",Time_now->tm_year+1900,Time_now->tm_mon+1,Time_now->tm_mday,Time_now->tm_hour,Time_now->tm_min ,Time_now->tm_sec );
	sprintf(buffer,"%lf",tv2s(&now) );
	
	date_str= buffer;

	return date_str;
}
 
string get_ip_port(int connfd){
	uint32_t len=sizeof(SA);
    sockaddr_in get_ip_port;
	char buffer[32];
	string _ip_port;
    if (getpeername(connfd, (struct sockaddr *)&get_ip_port, &len) == -1)
    		perror("getsockname");
		else{
			sprintf(buffer,"%s:%u", inet_ntoa(get_ip_port.sin_addr) ,ntohs(get_ip_port.sin_port));
			
			 _ip_port =buffer;
			 //cout<< "* client connected from "<<_ip_port; 
		}
	return _ip_port;
}
void set_map(int connfd){
 
	map_Name_Ip_and_port[connfd][0]= "";
	 
	



}
void erase_map(int fd ){
	map<int, string[2]>::iterator iter;
	iter = map_Name_Ip_and_port.find(fd);
	 
	cout   <<"* client" <<" disconnected"<<endl;

    map_Name_Ip_and_port.erase(iter);
 
}


void set_sink_map(int connfd){
 
	map_sink_Name_Ip_and_port [connfd][0]= "";
	 
	



}
void erase_sink_map(int fd ){
	map<int, string[2]>::iterator iter;
	iter = map_sink_Name_Ip_and_port.find(fd);
	 
	cout  <<"* client"<<" disconnected"<<endl;

    map_sink_Name_Ip_and_port.erase(iter);
 
}
 
//get_online_users();
 
void simple_tokenizer(string s,vector<string>& arg)
{
    stringstream ss(s);
    string word;
    while (ss >> word) {
		arg.push_back(word);
        cout << word << endl;
    }
}
 
void deal_input(int sockfd ,char* buf,int size ){
	string message (buf,size);
	map<int, string[2]>::iterator iter;

	if	(message=="/reset\n"){
		
		timeval now;
	// time_t now = time(NULL);
		gettimeofday(&now,NULL); 
		reset_time=tv2s(&now);
		
		string p_message=to_string(reset_time)+ " RESET "+to_string(counter)+" \n";
		counter=0;
		write(sockfd,p_message.c_str(),p_message.length() );
		 printf("%lf \n",reset_time);
	}
	else if	(message=="/report\n"){
		
		struct timeval _t1;
		double t0, t1;
		gettimeofday(&_t1, NULL);
	
		t1 = tv2s(&_t1);
		t0= reset_time;
		char buffer[OPEN_MAX]; 
		sprintf(buffer ,"%lu.%06lu REPORT %llu %.6fs %.6f Mbps\n",
		_t1.tv_sec, _t1.tv_usec, counter, t1-t0, 8.0*(counter/1000000.0)/(t1-t0) );
		string p_message =buffer;
		write(sockfd,p_message.c_str(),p_message.length() );
 
	}
	else if (message=="/ping\n"){
		string p_message=date_get()+ " PONG\n";
	 
		write(sockfd,p_message.c_str(),p_message.length() );
	}
	else if (message=="/clients\n"){///change to new map
		string p_message=date_get()+ " CLIENTS "+ to_string(map_sink_Name_Ip_and_port.size())+"\n";
	 
		write(sockfd,p_message.c_str(),p_message.length() );
	}
	 
}


// Driver function
int main(int argc, char * argv[])
{
	int i, maxi, listenfd, connfd, sockfd;
	int  sink_i, sink_maxi,sink_listenfd, sink_connfd,sink_sockfd;
	int nready,sink_nready;
	ssize_t n,sink_n;
	char buf[MAXLINE];
	char sink_buf[MAXLINE];
	socklen_t clilen;
	socklen_t sink_clilen;
	struct pollfd client[OPEN_MAX];
	struct pollfd sink_client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;
	struct sockaddr_in sink_cliaddr, sink_servaddr;
 
	 
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
	listenfd = socket(AF_INET, SOCK_STREAM, 0);


	if (sink_listenfd == -1) {
		printf("sink_socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));
	sink_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	// assign IP,
	uint32_t port =std::atoi(argv[1]) ;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	 
	sink_servaddr.sin_family = AF_INET;
	sink_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sink_servaddr.sin_port = htons(port+1);

	// Binding newly created socket to given IP and verification
	if ((bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		perror("bind");
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");


	if ((bind(sink_listenfd, (SA*)&sink_servaddr, sizeof(sink_servaddr))) != 0) {
		perror("bind");
		printf("sink_socket bind failed...\n");
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


	//setcokfd rcv buf

	if ((listen(sink_listenfd, 256)) != 0) {
		printf("sink_Listen failed...\n");
		exit(0);
	}
	else
		printf("sink_Server listening..\n");


	// Accept the data packet from client and verification




	client[0].fd = listenfd;
	client[1].fd = sink_listenfd;
	 
	client[0].events = POLLRDNORM;
	client[1].events = POLLRDNORM;
	for (i = 2; i < OPEN_MAX; i++)
		client[i].fd = -1; /* -1: available entry */
	maxi = 0; /* max index in client[] */

	// sink_client[0].fd = sink_listenfd;
	// sink_client[0].events = POLLRDNORM;
	// for (sink_i = 1; sink_i < OPEN_MAX; sink_i++)
	// 	sink_client[sink_i].fd = -1; /* -1: available entry */
	// sink_maxi = 0; /* max index in client[] */

	for ( ; ; ){


		 
		
		
		nready = poll(client, maxi+1, INFTIM);
		 
		if (client[0].revents & POLLRDNORM) { /* new client */
			printf("server accept the client...\n");
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
			
			//


			// before adding
			//get_online_users(client,maxi);

			//broadcast_rest_user(coonfd,);
			for (i = 2; i < OPEN_MAX; i++)
				if (client[i].fd < 0) {
				client[i].fd = connfd; /* save descriptor */
				break;
				}
			if (i == OPEN_MAX) printf("too many clients");//err_quit("too many clients");



			client[i].events = POLLRDNORM;



			if (i > maxi) maxi = i; /* max index in client[] */
			
			
			int tmp=connfd;
			set_map(tmp );
			
			 --nready;

			
		}
		 
		else if (client[1].revents & POLLRDNORM) { /* new client */
			printf("server accept the sink_client...\n");
			sink_clilen = sizeof(sink_cliaddr);
			sink_connfd = accept(sink_listenfd, (SA *) &sink_cliaddr, &sink_clilen);
			
			//
			

 
			for (i = 2; i < OPEN_MAX; i++)
				if (client[i].fd < 0) {
				client[i].fd = sink_connfd; /* save descriptor */
				break;
				}
			if (i == OPEN_MAX) printf("too many clients");//err_quit("too many clients");



			client[ i].events = POLLRDNORM;



			if (i > maxi) maxi =  i; /* max index in client[] */
			
			
			int tmp=sink_connfd;
			set_sink_map(tmp );
			 --nready;
			
		}
	

		if ( nready<= 0) continue; /* no more readable descs */


		//printf("start checkinng \n");
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
					//printf("client  %i shutdown \n" ,sockfd );
					int tmp=sockfd;
					if (map_sink_Name_Ip_and_port.find(tmp)==map_sink_Name_Ip_and_port.end()){
						erase_map(tmp );
						
					}
					else{
						erase_sink_map(tmp);
					}
					close(sockfd);
					client[i].fd = -1;
				}
				else{
					//printf("%s \n",buf);
					int tmp=sockfd;

					if (map_sink_Name_Ip_and_port.find(tmp)==map_sink_Name_Ip_and_port.end()){

						deal_input( sockfd ,buf,n );
						}
					else{
						counter+=n;
						 
					}
					//write(sockfd, buf, n);
				}

				if (--nready <= 0) break; /* no more readable descs */
			}
		}

		///sink_server
		
		 	
		
		// for (sink_i = 1; sink_i <= sink_maxi; sink_i++) {
		// 	/* check all clients for data */
		// 	if ( (sink_sockfd = sink_client[i].fd) < 0) continue;
		// 	if (sink_client[i].revents & (POLLRDNORM | POLL_ERR)) {
		// 		if ( (n = read(sockfd, buf, MAXLINE)) < 0) {
		// 			if (errno == ECONNRESET) {
		// 				/* connection reset by client */
		// 				close(sink_sockfd);
		// 				client[sink_i].fd = -1;
		// 				}
		// 			else printf("read error");
		// 		}
		// 		else if (n == 0) { /* connection closed by client */
		// 			//printf("client  %i shutdown \n" ,sockfd );
		// 			int tmp=sink_sockfd;
		// 			erase_sink_map(tmp );
		// 			close(sink_sockfd);
		// 			sink_client[i].fd = -1;
		// 		}
		// 		else{
		// 			printf("%s \n",buf);
		// 			//deal_sink_input( sink_sockfd ,buf,n );
		// 			counter+=n;
					
		// 			write(sink_sockfd, buf, n);
		// 		}

		// 		if (--sink_nready <= 0) break; /* no more readable descs */
		// 	}
		// }
		


	}


	// Function for chatting between client and server
	//func(connfd);

	// After chatting close the socket

	close(sockfd);
	close(sink_sockfd);
}