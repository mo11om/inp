 
#include <arpa/inet.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
 
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/signal.h>

 

#define PORT 10003
static struct timeval _t0;
static unsigned long long bytesent = 0;
using namespace std;
int main(int argc, char const* argv[])
{
	 
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	
	char buffer[1000] = { 0 };


	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}
    uint32_t port =std::atoi(argv[2]) ;
    string address = (argv[1]) ;
    cout<< address <<": "<<port<<endl;
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr( address.c_str());
	serv_addr.sin_port = htons(port);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
 

	if ((connect(sock, (sockaddr*)&serv_addr,	sizeof(serv_addr)))) {
		printf("\nConnection Failed \n");
		return -1;
	} 
    string Ping="/ping\n";
    write(sock,Ping.c_str(),Ping.length());
	read(sock, buffer, sizeof(buffer));
    cout<<buffer;
	// signal(SIGINT,  handler);
	// signal(SIGTERM, handler);

	
	 
	 
	// char* p;
	// float  byterate = stof (argv[1] );
	// byterate= 999*byterate;
	// int send_rate = (int)byterate;
	// bool flag =true;
	// printf("%d \n", send_rate);
	// while(1) {
	// 	 timeval t_start ;
	// 	 gettimeofday(&t_start, NULL);
	// 	for (int i = 0 ; i<send_rate ;i++ ){
	// 		bytesent += send(sock, buffer, 1000, 0);
	// 		// struct timespec t_c = { 0, 1 };

			
	// 	} 
	    	
	// 	// nanosleep(&t_c, NULL);
	// 	timeval  t_end;
		
	// 	long wait_time ;
	// 	gettimeofday(&t_end, NULL);
	// 	if (flag){
	// 		wait_time=1000000000 * (1-(tv2s(&t_end)-(tv2s(&_t0))));
	// 		flag=false;
	// 	}
	// 	else 
	// 		wait_time=1000000000 * (1-(tv2s(&t_end)-(tv2s(&t_start))));
	// 	//printf("%ld   \n" , wait_time); 
        
	// 	struct timespec t = { 0, wait_time};

	// 	nanosleep(&t, NULL);
		
	// }
         

     
  


    
	close(sock);
	return 0;
}
