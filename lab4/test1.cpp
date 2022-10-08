// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#define PORT 10002
using namespace std;
int main(int argc, char const* argv[])
{
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	
	char buffer[1024] = { 0 };
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
    valread = recv(sock, buffer, 1024,0);
	printf("%s\n", buffer);
    string hello = "GO\n";
	send(sock, hello.c_str(), hello.length(), 0);
	printf("Hello message sent\n");
	
    printf("start calculate\n");
     
	
     
    int total_read=-40;
    string str_check="";
    int tolerant=4; 
    char buf;  
    while (tolerant){ 
        valread = recv(sock,  &buf, 1,0);
         
        // perror("Error: "); 
        //cout<<"byte eat"<<total_read << endl;
        str_check = str_check + buf ;
    
        //str_check+=buf;
       
        total_read+= valread; 
        //printf("%s\n", buffer);
        //cout<<str_check<<" ";
        if(str_check =="="|str_check =="=="|str_check =="==="|str_check =="===="){
              if(str_check =="===="){
                tolerant--;
              }
              continue;
        }

         str_check.clear();

    }


    memset(buffer, 0, sizeof(buffer));
    valread= recv(sock ,buffer,1024,0);  
    printf("%s", buffer);
    // tolerant =1;
    // while (tolerant){ 
    //     valread = recv(sock,  &buf, 1,0);\
         
    //     // perror("Error: "); 
    //     //cout<<"byte eat"<<total_read << endl;
    //     str_check = str_check + buf ;
    
    //     //str_check+=buf;
       
          
    //     //printf("%s\n", buffer);
    //     cout<<str_check;
    //     if(str_check =="?" ){
    //           tolerant=0;
    //           continue;
    //     }

    //      str_check.clear();
    // }
   
    
    string ans = to_string(total_read);
    ans+="\n";
    cout<<"ans"<<ans<<endl;
    
    valread=send(sock, ans.c_str(), ans.length(), 0);
     
    cout<<valread<<endl;
    
    memset(buffer, 0, sizeof(buffer));
    valread= recv(sock ,buffer,1024,0); 
    printf("valread %d \n",valread  );

    printf("valread %d %s\n",valread , buffer);
    
	
	// closing the connected socket
	close(client_fd);
	return 0;
}
