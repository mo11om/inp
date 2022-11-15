 
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
#include <vector>

using namespace std;

vector<int > cmd_data_connections;


int connection_num=40;

void handler(int s) {
    char buffer[1000] = { 0 };
    string REPORT="/report\n";
	write(cmd_data_connections.at(0),REPORT.c_str(),REPORT.length());
	read(cmd_data_connections.at(0), buffer, sizeof(buffer));
    cout<<buffer;
    for (int i=0;i<connection_num;i++){
        
        close(cmd_data_connections.at(i));
    } 
 	exit(0);


}
void send(){
    char buffer[1000] = {0};

    while(1) {
         for (int i=1;i<connection_num;i++) {
         
             write(cmd_data_connections.at(i), buffer, 500);
         }

		
	 
	    	
	 
		 
	}
}
int get_new_Data_connect_(string address,uint32_t port){
    int sock , valread, client_fd;
	struct sockaddr_in serv_addr;
	
	char buffer[1000] = { 0 };


	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		exit(-1);
	}
     
    
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr( address.c_str());
	serv_addr.sin_port = htons(port);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
    

    ///set send buffer?

	long rc_buf =10000000 ;
   	setsockopt(sock, SOL_SOCKET,SO_SNDBUF,&rc_buf,sizeof(rc_buf));

	if ((connect(sock, (sockaddr*)&serv_addr,	sizeof(serv_addr)))) {
		printf("\nConnection Failed \n");
		exit(-1);
	} 
    return sock;

}
int main(int argc, char const* argv[])
{
	
	int sock , valread, client_fd;
	struct sockaddr_in serv_addr;
	
	char buffer[1000] = { 0 };


	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}
    uint32_t port =std::atoi(argv[2]) ;
    string address = (argv[1]) ;
   
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr( address.c_str());
	serv_addr.sin_port = htons(port);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
 

	if ((connect(sock, (sockaddr*)&serv_addr,	sizeof(serv_addr)))) {
		printf("\nConnection Failed \n");
		return -1;
	} 
    
    cmd_data_connections.push_back(sock);


    

    string RESET="/reset\n";

    write(sock,RESET.c_str(),RESET.length());
	read(sock, buffer, sizeof(buffer));
    cout<<buffer;

    for (int i=0 ;i<connection_num;i++){
         client_fd=  get_new_Data_connect_(address,port+1);
    
        cmd_data_connections.push_back(client_fd);
    }
    
   

    //for (int i=0;i<cmd_data_connections.size();i++) cout<< cmd_data_connections.at(i)<<endl;


    
    signal(SIGKILL, handler); 
	
    signal(SIGTERM, handler); 
	 
	send();
	
 
 
 
	 
 
	
         

     
  


   
	
	return 0;
}
