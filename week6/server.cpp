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
#include <sstream>
#include <vector>
#include <map>
#include "aa.h"
#define MAXLINE 500
#define Animal_Num 889
#define adj_Num 3434
#define SA struct sockaddr
#define OPEN_MAX 1024
#define INFTIM -1
using namespace std;
string date_get(  ){
	string date_str;
    char buffer[32];
	

	time_t now = time(NULL);
	 
   // convert now to string form
   	struct tm *Time_now = localtime(&now);
	
	sprintf(buffer,"%i-%i-%i %i:%i:%i *** ",Time_now->tm_year+1900,Time_now->tm_mon+1,Time_now->tm_mday,Time_now->tm_hour,Time_now->tm_min ,Time_now->tm_sec );
	date_str= buffer;
	return date_str;
}

string random_nick_name(  ){
	time_t now = time(0);
	string name_str;
    char buffer[32];
   // convert now to string form
   	struct tm *Time_now = localtime(&now);
    srand ( time(NULL) );

	int AniIndex=  random ()%Animal_Num;
    int adjIndex = random ()%adj_Num;
	sprintf(buffer,"%s %s",adjs[adjIndex],animals[AniIndex]);
	name_str= buffer;
	return name_str;
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
void set_map(int connfd,map<int, string[2]> & map_Name_Ip_and_port){
	string  ip_port_address = get_ip_port(connfd);
	string   nickname = random_nick_name();
	cout<<"* client connected from "<< ip_port_address<<endl;
	map_Name_Ip_and_port[connfd][0]= nickname;
	map_Name_Ip_and_port[connfd][1]=   ip_port_address  ;

	



}
void erase_map(int fd,map<int, string[2]> & map_Name_Ip_and_port ){
	map<int, string[2]>::iterator iter;
	iter = map_Name_Ip_and_port.find(fd);
	 
	cout<< "* client"<< iter->second[1] <<" disconnected"<<endl;

    map_Name_Ip_and_port.erase(iter);

}
string nick_name_get(int fd,map<int, string[2]> & map_Name_Ip_and_port ){
	map<int, string[2]>::iterator iter;
	iter = map_Name_Ip_and_port.find(fd);
	string name;
    if(iter != map_Name_Ip_and_port.end())
        name =iter->second[0];
    else
       cout<<"Do not Find"<<endl;

	return name;
}
//get_online_users();
void second_message(int connfd,pollfd* client,int maxi,map<int, string[2]> & map_Name_Ip_and_port){
		int total=0;
		string nick_name,date_str,s_message;

		// for (int i = 1; i <= maxi; i++) {
		// 	cout<< client[i].fd;
		// 	 if (client[i].fd < 0) {
		// 		 continue;
		// 	}
		// 	total++;
		// }



		date_str=date_get();
		nick_name=nick_name_get(connfd,map_Name_Ip_and_port);
		s_message= date_str + " Total "+to_string(map_Name_Ip_and_port.size())+" users online now. Your name is <"+nick_name+">\n";

		write(connfd,s_message.c_str(), s_message.length());

}

void hello_client(int connfd)
{
	///char buff[MAXLINE] ;




	string date_str =date_get();

	date_str+=" Welcome to the simple CHAT server \n";
	//std::cout<<date_str;
	write(connfd,date_str.c_str(), date_str.length());



    //printf("%s \n"  ,buffer);

	// infinite loop for chat
	// for (;;) {


	// }
}
void simple_tokenizer(string s,vector<string>& arg)
{
    stringstream ss(s);
    string word;
    while (ss >> word) {
		arg.push_back(word);
        cout << word << endl;
    }
}
void deal_input(int sockfd ,char* buf,int size,map<int, string[2]> &map_Name_Ip_and_port){
	string message (buf,size);
	map<int, string[2]>::iterator iter;
	string split="--------------------------------------------------\n";

	if	(message.find("/name") != std::string::npos){
		vector<string> arg;
		 simple_tokenizer(message,arg);
		 
		 if(arg.size()==3){
			iter=map_Name_Ip_and_port.find(sockfd);
			iter->second[0]=arg[1]+" "+arg[2];
		 }
		 else{
			string  s_message=   date_get()+"Unknown or incomplete command </name>"+"\n";
			write(sockfd,s_message.c_str(),s_message.length() );
		 }

	}
	else if(message.find("/who") != std::string::npos){
		write(sockfd,split.c_str(),split.length() );

		for(iter = map_Name_Ip_and_port.begin(); iter != map_Name_Ip_and_port.end(); iter++){
			
			
			string s_message= iter->second[0]+"\t"+iter->second[1]+"\n";

			
			if (iter->first==sockfd) s_message= "* "+s_message;
			
			
			write(sockfd,s_message.c_str(),s_message.length() );
		}
		write(sockfd,split.c_str(),split.length() );

	}
	else {
			iter=map_Name_Ip_and_port.find(sockfd);
			string  s_message=  "<"+iter->second[0]+">"+"\t"+date_get()+message;
			// s_message= ;
			
			//s_message=  ;
		 	for(iter = map_Name_Ip_and_port.begin(); iter != map_Name_Ip_and_port.end(); iter++)
				if (iter->first!=sockfd){
					
					write(iter->first,s_message.c_str(),s_message.length() );
				}
                
	}

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
	map<int, string[2]> map_Name_Ip_and_port;
	setenv("TZ", "GMT-8", 1);
	tzset();
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
			
			//


			// before adding
			//get_online_users(client,maxi);

			//broadcast_rest_user(coonfd,);
			for (i = 1; i < OPEN_MAX; i++)
				if (client[i].fd < 0) {
				client[i].fd = connfd; /* save descriptor */
				break;
				}
			if (i == OPEN_MAX) printf("too many clients");//err_quit("too many clients");



			client[i].events = POLLRDNORM;



			if (i > maxi) maxi = i; /* max index in client[] */
			
			
			int tmp=connfd;
			set_map(tmp,map_Name_Ip_and_port);
			
			 
			hello_client (tmp );

			second_message( tmp ,client , maxi,map_Name_Ip_and_port);

			if (--nready <= 0) continue; /* no more readable descs */
		}
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
					erase_map(tmp,map_Name_Ip_and_port);
					close(sockfd);
					client[i].fd = -1;
				}
				else{
					//printf("%s \n",buf);
					deal_input( sockfd ,buf,n, map_Name_Ip_and_port);
					//write(sockfd, buf, n);
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