 
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

double tv2s(struct timeval *ptv) {
	return 1.0*(ptv->tv_sec) + 0.000001*(ptv->tv_usec);
}

void handler(int s) {
	struct timeval _t1;
	double t0, t1;
	gettimeofday(&_t1, NULL);
	t0 = tv2s(&_t0);
	t1 = tv2s(&_t1);
	fprintf(stderr, "\n%lu.%06lu %llu bytes sent in %.6fs (%.6f Mbps; %.6f MBps)\n",
		_t1.tv_sec, _t1.tv_usec, bytesent, t1-t0, 8.0*(bytesent/1000000.0)/(t1-t0), (bytesent/1000000.0)/(t1-t0));
	exit(0);
}
using namespace std;
int main(int argc, char const* argv[])
{
	gettimeofday(&_t0, NULL);
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	
	char buffer[1000] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "140.113.213.213", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((client_fd
		= connect(sock, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	} 
    
	
	signal(SIGINT,  handler);
	signal(SIGTERM, handler);

	
	 
	 
	char* p;
	float  byterate = stof (argv[1] );
	byterate= 1000*byterate;
	int send_rate = (int)byterate;
	bool flag =true;
	printf("%d \n", send_rate);
	while(1) {
		 timeval t_start ;
		 gettimeofday(&t_start, NULL);
		for (int i = 0 ; i<send_rate ;i++ ){
			bytesent += send(sock, buffer, 1000, 0);
			// struct timespec t_c = { 0, 1 };

			
		} 
	    	
		// nanosleep(&t_c, NULL);
		timeval  t_end;
		
		long wait_time ;
		gettimeofday(&t_end, NULL);
		if (flag){
			wait_time=1000000000 * (1-(tv2s(&t_end)-(tv2s(&_t0))));
			flag=false;
		}
		else 
			wait_time=1000000000 * (1-(tv2s(&t_end)-(tv2s(&t_start))));
		//printf("%ld   \n" , wait_time); 

		struct timespec t = { 0, wait_time};

		nanosleep(&t, NULL);
		
	}
         

     
  


    
	close(client_fd);
	return 0;
}
