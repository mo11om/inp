#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>      
#include <sys/un.h>
#include <string>
#include <iostream>
#include "get_init_array.h"


#define MAX 1024
#define SA  sockaddr;



using namespace std;



 


void give_value(int sockfd,vector<vector<int>> place_need_to_set , int grid[9][9] )
{
    for (int i = 0; i < 9; i++)
	{
        for (int j = 0; j <   place_need_to_set.at(i).size(); j++)
             {
                string send="v ";
                
                
                send += to_string( (i));
                send += " ";
                send += to_string( place_need_to_set.at(i).at(j));
                send += " ";

                send += to_string( (  grid[i][place_need_to_set.at(i).at(j)]));
                cout<<send;
                write(sockfd, send.c_str(), send.length());
                
                
                char buff[MAX];
                bzero(buff, sizeof(buff));
                
                read(sockfd, buff, sizeof(buff));
                printf("From Server : %s", buff);
             }
	}
}

void func(int sockfd)
{
    string send="s";
    
    vector<vector<int>> place_need_to_set(9);int grid[9][9];
    char buff[MAX];
     
     
     
     
    write(sockfd, send.c_str(), send.length());
    bzero(buff, sizeof(buff));
    
    int n=read(sockfd, buff, sizeof(buff));
    buff[n]='\0';
    printf("From Server : %s", buff);
    string rec_grid=buff;
    cout<<rec_grid.length()<<"rec_\n"<<rec_grid<<endl;
    slover(rec_grid,  place_need_to_set , grid );

    give_value(sockfd,  place_need_to_set , grid );



     

     send="c";


    write(sockfd, send.c_str(), send.length());
    
    bzero(buff, sizeof(buff));read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);



     
}

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_un servaddr;
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
 
    
    strcpy(servaddr.sun_path, argv[1]);

    connect(sockfd, ( sockaddr *) &servaddr, sizeof(servaddr));
    
    func(sockfd);    
    // str_cli(stdin, sockfd); /* do it all */
    return(0);
}
