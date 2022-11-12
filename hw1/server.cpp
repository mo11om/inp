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
#include <string> 
#include <istream>
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



//map all member 
map<int, string[2]> map_Name_Ip_and_port;
map <int ,vector<int>> map_fd_channels;


struct channel{
	string name="";
	string topic="";
	map <int ,string> map_channel_fd_nickname;
};

vector<channel*> all_channels;

 

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
			sprintf(buffer,"%s", inet_ntoa(get_ip_port.sin_addr));
			
			 _ip_port =buffer;
			 //cout<< "* client connected from "<<_ip_port; 
		}
	return _ip_port;
}
void set_map(int connfd){
	string  ip_port_address = get_ip_port(connfd);
	string   nickname = random_nick_name();
	cout<<"* client connected from "<< ip_port_address<<endl;
	map_Name_Ip_and_port[connfd][0]= "";
	map_Name_Ip_and_port[connfd][1]=   ip_port_address  ;

	



}
void erase_map(int fd ){
	map<int, string[2]>::iterator iter;
	iter = map_Name_Ip_and_port.find(fd);
	 
	cout<< "* client"<< iter->second[1] <<" disconnected"<<endl;

    map_Name_Ip_and_port.erase(iter);
	  
	// for(iter = map_Name_Ip_and_port.begin(); iter != map_Name_Ip_and_port.end(); iter++){
	// 		int sockfd=iter->first;
	// 		write(sockfd,s_message.c_str(),s_message.length() );
	// 	}

}
string nick_name_get(int fd ){
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


 void set_server(int sockfd,string nickname){
	 
	cout<<sockfd <<nickname <<endl;
	vector<int> tmp_vec ;
	tmp_vec.push_back(0);
	map_fd_channels[sockfd]=tmp_vec;


	if (all_channels.empty()){
		channel* server= new channel;
		server->name= "server";
		server->topic= "server_topic";
		server->map_channel_fd_nickname[sockfd]=nickname;
		all_channels.push_back(server);
	}

	else{
		all_channels[0]->map_channel_fd_nickname[sockfd]=nickname;
		
	}

	map <int ,vector<int>>::iterator iter;
	for( iter= map_fd_channels.begin(); iter != map_fd_channels.end(); iter++) {
		cout<<iter->first;
		for(int i=0; i<iter->second.size(); i++) cout<<i<<" " << iter->second.at(i) << " ";
		cout<<endl;
		
	}
	cout<<endl;
 

}


void erase_server(int fd ){
	map<int, vector <int>>::iterator iter ;
	vector <int>::iterator channel_s;
	 
	iter = map_fd_channels.find(fd);
	
	for(channel_s=iter->second.begin(); channel_s!=iter->second.end();channel_s++ )
	{
		cout<<"clean channel "<<*channel_s<<endl;
		 
		all_channels[*channel_s]->map_channel_fd_nickname.erase(fd);
		 
		
	} 
	cout<<fd;
	map_fd_channels.erase(fd);

	// for(iter = map_Name_Ip_and_port.begin(); iter != map_Name_Ip_and_port.end(); iter++){
	// 		int sockfd=iter->first;
	// 		write(sockfd,s_message.c_str(),s_message.length() );
	// 	}

}

void list_one_channels(int sockfd,string channel_name){
	string RPL_LISTSTART	=":mircd  321 "+nick_name_get(sockfd)+" Channel :Users  Name \r\n";
	write(sockfd,RPL_LISTSTART.c_str(),RPL_LISTSTART.length() );



	for (int i = 1 ;i<all_channels.size();i++){//要改0->1
		if(all_channels.at(i)->name ==channel_name){
				string channel_info= ":mircd 322 "+nick_name_get(sockfd)+" "+all_channels.at(i)->name+" #"+to_string(all_channels.at(i)->map_channel_fd_nickname.size())+" :"+all_channels.at(i)->topic +"\r\n";
			cout<< "channels "<< channel_info<<endl;
			write(sockfd,channel_info.c_str(),channel_info.length() );
		}
		

	}
	string RPL_LISTEND	=":mircd  323 "+nick_name_get(sockfd)+" Channel :End of /LIST \r\n";
	write(sockfd,RPL_LISTEND.c_str(),RPL_LISTEND.length() );

} 
void list_channels(int sockfd){
	string RPL_LISTSTART	=":mircd  321 "+nick_name_get(sockfd)+" Channel :Users  Name \r\n";
	write(sockfd,RPL_LISTSTART.c_str(),RPL_LISTSTART.length() );



	for (int i = 1 ;i<all_channels.size();i++){//要改0->1
		string channel_info= ":mircd 322 "+nick_name_get(sockfd)+" "+all_channels.at(i)->name+" #"+to_string(all_channels.at(i)->map_channel_fd_nickname.size())+" :"+all_channels.at(i)->topic +"\r\n";
		cout<< "channels "<< channel_info<<endl;
		write(sockfd,channel_info.c_str(),channel_info.length() );

	}
	string RPL_LISTEND	=":mircd  323 "+nick_name_get(sockfd)+" Channel :End of /LIST \r\n";
	write(sockfd,RPL_LISTEND.c_str(),RPL_LISTEND.length() );

}

int  add_channel(int sockfd,string nickname,string channel_name){
	int i;
	for ( i= 1 ;i<all_channels.size();i++){
		if (all_channels.at(i)->name==channel_name){
			all_channels.at(i)->map_channel_fd_nickname[sockfd]=nickname;
			map_fd_channels[sockfd] .push_back(i);
			return i;
		}
	}
	if (i==all_channels.size()){
		channel* tmp = new channel;
		tmp->name= channel_name;
		tmp->map_channel_fd_nickname[sockfd]=nickname;
		all_channels.push_back(tmp);
		map_fd_channels[sockfd].push_back(i);
		
	}
	return i;
}

 

void names_channel(int sockfd ,string channel_name){
	for (int i = 1 ;i<all_channels.size();i++){
		if (channel_name== all_channels[i]->name){
			map <int ,string> ::iterator  iter;

			for(iter=all_channels[i]->map_channel_fd_nickname.begin();iter!=all_channels[i]->map_channel_fd_nickname.end();iter++){
					string name =":mircd 353 "+nick_name_get(sockfd)+" #"+channel_name+" :"+iter->second+"\r\n"; 
					write(sockfd,name.c_str(),name.length() );
			}
			
			
			break;
		}
		
	}

	string end = ":mircd 366 "+nick_name_get(sockfd)+" #"+channel_name+" :End of Names List\r\n";
	cout<<end;
	write(sockfd,end.c_str(),end.length() );

}
void join_channel(int sockfd,string nickname,string channel_name){
	int index=add_channel(sockfd, nickname, channel_name);
	 string has_joined=":"+nickname+" JOIN #"+channel_name+"\r\n";
	 string  topic_set="";
	if (all_channels[index]->topic==""){
		topic_set=":mircd 331 "+nickname+" #"+channel_name+" :No topic is set\r\n";
	}
	else{
		topic_set=":mircd 331 "+nickname+" #"+channel_name+" :"+ all_channels.at(index)->topic+"\r\n";

	}
	cout<< topic_set<<endl;
	write(sockfd,has_joined.c_str(),has_joined.length() );
	write(sockfd,topic_set.c_str(),topic_set.length() );

	names_channel(sockfd,channel_name);
}


void send_users(int sockfd){
	 
	map <int ,string>::iterator iter;
	string RPL_USERSSTART	=":mircd  392  "+nick_name_get(sockfd)+"  :UserID Terminal Host \r\n";
	write(sockfd,RPL_USERSSTART.c_str(),RPL_USERSSTART.length() );
	
	for( iter=  all_channels[0]->map_channel_fd_nickname.begin();
		iter!=  all_channels[0]->map_channel_fd_nickname.end();
		iter++ )
	{
		 
		string user_info=":mircd  393  "+nick_name_get(sockfd)+"  :"+iter->second+" - "+get_ip_port(iter->first)+"\r\n";
		 write(sockfd,user_info.c_str(),user_info.length() );
		cout<<user_info<<endl;

		 
	} 
	string RPL_ENDOFUSERS	=":mircd  394   "+nick_name_get(sockfd)+"  :End of user\r\n";
	 write(sockfd,RPL_ENDOFUSERS.c_str(),RPL_ENDOFUSERS.length() );
}
void  not_on_channel(int sockfd){
	":mircd  442 "+nick_name_get(sockfd)+"   :You're not on that channel\r\n";

}


void topic_view(int sockfd,string nickname,string channel_name){
	
	for (int i = 1 ;i<all_channels.size();i++){
		if (channel_name== all_channels[i]->name){
			string topic_set="";
			if(all_channels[i]->topic==""){
				topic_set=":mircd 331 "+nickname+" #"+channel_name+" :No topic is set\r\n";
			}
			else{
				topic_set=":mircd 332  "+nickname+" #"+channel_name+" :"+ all_channels.at(i)->topic+"\r\n";
	
			}
			write(sockfd,topic_set.c_str(),topic_set.length() );
			break;
		}
	}
}
void topic_change(int sockfd,string nickname,string channel_name,string topic_name){
	
	
	for (int i = 1 ;i<all_channels.size();i++){
		if (channel_name== all_channels[i]->name){
			 
			all_channels[i]->topic= topic_name;
			 
			break;
		}
	}
	topic_view(sockfd ,nickname,channel_name);

}
void part_channel(int fd ,string nickname ,string channel_name){
	cout<<"part_m"<<endl;
	for (int i = 1 ;i<all_channels.size();i++){
		if (channel_name== all_channels[i]->name){
			 
			string part_m= ":"+nickname+" PART :#"+channel_name+"\r\n";
			all_channels[i]->map_channel_fd_nickname.erase(fd);

			map <int ,string> ::iterator  iter;
			write(fd,part_m.c_str(),part_m.length() );
			for(iter=all_channels[i]->map_channel_fd_nickname.begin();iter!=all_channels[i]->map_channel_fd_nickname.end();iter++){
			
			 cout<<part_m;
			 write(iter->first,part_m.c_str(),part_m.length() );
			}

			 
			break;
		}
	}
}

void privmsg(int sockfd,string nickname,string channel_name,string m_send){

	for (int i = 1 ;i<all_channels.size();i++){
		if (channel_name== all_channels[i]->name){
			map <int ,string> ::iterator  iter;
			for(iter=all_channels[i]->map_channel_fd_nickname.begin();iter!=all_channels[i]->map_channel_fd_nickname.end();iter++){
					if(iter->first!= sockfd)
					{string message  =":"+nickname+" PRIVMSG #"+channel_name+" :" +m_send +  "\r\n"; 
					cout<<iter->first <<message;
					write(iter->first,message.c_str(),message.length() );}
			}

		}
	}
}
void split_instructions (string strlist,vector<string>& cmd){
 
    std::istringstream MyStream(strlist);
  
    std::string s;
     while (std::getline(MyStream, s))
     {
        cmd.push_back(s);
		 cout<< s<<endl;
         
     }
      
}
void simple_tokenizer(string s,vector<string>& arg)
{
    stringstream ss(s);
    string word;
    while (ss >> word) {
		arg.push_back(word);
        // cout<<word<<endl; 
    }
}
 
bool check_nickname(string nickname){
	bool nick_collision=false;
	map<int, string[2]>::iterator iter;
	for(iter = map_Name_Ip_and_port.begin(); iter != map_Name_Ip_and_port.end(); iter++){
			cout<< iter->second[0]<<endl;
			if (iter->second[0] == nickname){
				nick_collision =true;
				break;

			}

		}
		 
	return nick_collision;

}
string need_more_arg(string nickname,string command ){
	string respond =":mircd 461 "+nickname+command+ " :Not enough parameters";
	 
	return respond;
}
 
void deal_input(int sockfd ,char* buf,int size ){
	string message (buf,size);
	 

	vector<string> cmd,arg;
	 
	
	split_instructions(message,cmd);
	
	string nickname=nick_name_get(sockfd);
	map<int, string[2]>::iterator iter;
	 
	

 	
	


	
	



	for (int i=0; i<cmd.size(); i++)
	{	
		arg.clear();
		simple_tokenizer(cmd.at(i),arg);
		for(int i=0; i<arg.size(); i++) cout<<i<<" " << arg.at(i) << " ";
		cout<<endl;
		if	( arg[0]=="NICK"  ){
			if (arg.size()== 1){
				string no_give_nick=":mircd 251 "+nickname+" :No nickname given\r\n";

				//write(sockfd, no_give_nick .c_str(), no_give_nick .length());
				 
			}
			else if (check_nickname(arg[1])){

				string nick_collision=":mircd  436 "+arg[1]+" "+nickname +" :Nickname collision KILL\r\n";
						 
				write(sockfd, nick_collision .c_str(), nick_collision .length());

			}
			else{
				cout<< " nick receive \n" ;
				map_Name_Ip_and_port[sockfd][0]=arg.at(1);
				 
				string motd 	= ":mircd 372 "+nickname+" :-  Hello, World! \r\n ";
			 
				 
				write(sockfd, motd .c_str(), motd.length());
			 
			}
			
			
			
				
		}
		else if(arg[0]=="USER" ){
			if (arg.size()< 5){
				string n_m_message= need_more_arg(nickname,arg.at(0));	
				write(sockfd, n_m_message .c_str(), n_m_message.length());
				
			}
			else{

				// add into server map


				set_server(sockfd,nickname);
				//send sucess message
				string us_num= to_string(map_fd_channels.size());
				cout<<"user receive"<<endl;
				 
				string Welcome	=":mircd 001 "+nickname+ " :Welcome to the minimized IRC daemon!\r\n";
				string users =":mircd 251 "+nickname+" :There are "+us_num+" users and 0 invisible on 1 server\r\n";
								
				string motd_start=":mircd 375 "+nickname+" :- mircd Message of the day -\r\n";
				string motd 	= ":mircd 372 "+nickname+" :-  Hello, World!\r\n";
				string motd_end =":mircd 376 "+nickname+" :End of message of the day\r\n";	

				write(sockfd, Welcome .c_str(), Welcome.length());
				write(sockfd, users .c_str(), users.length());
				write(sockfd, motd_start .c_str(), motd_start.length());
				write(sockfd, motd .c_str(), motd.length());
				write(sockfd, motd_end .c_str(), motd_end.length());

				
			}
			

			
	
			

		}
		else if(arg[0]=="QUIT"){
			erase_server(sockfd);
			
		}
		else if(arg[0]=="PING" ){
			if (arg.size()== 1){
				string ERR_NOORIGIN=":mircd 251 "+nickname+" :No nickname given\r\n";

				write(sockfd, ERR_NOORIGIN .c_str(), ERR_NOORIGIN.length());
				 
			}
			else{
				string PONG="PONG localhost\r\n";

				write(sockfd, PONG .c_str(), PONG.length());

			}
		}
		else if(arg[0]=="USERS" ){
			send_users(sockfd);
			
			
		}
		else if (arg[0]=="LIST"){
			if(arg.size()==1){
			list_channels(sockfd);

			}
			else if(arg.size()==2)
			{
				list_one_channels(sockfd,arg[1].erase(0,1));
			}

		}
		else if (arg[0]=="JOIN"){
				join_channel(sockfd,nickname,arg[1].erase(0,1));
		}
		else if (arg[0]=="NAMES"){
			if(arg.size()==1){

			}

			else if(arg.size()==2)
				names_channel(sockfd ,arg[1].erase(0,1));
		}
		else if (arg[0]=="TOPIC"){
			 
			if(arg.size()==2){
				topic_view(sockfd ,nickname,arg[1].erase(0,1));
			}
				
			else{
				string topic_name=arg[2].erase(0,1);
				for (int i = 3 ;i<arg.size(); i++){
					
					topic_name =topic_name + " "+ arg.at(i);
				}
				
				topic_change(sockfd ,nickname,arg[1].erase(0,1),topic_name);
			}
		 
		}
		else if (arg[0]=="PART"){
			if(arg.size()==1){

			}

			else if(arg.size()>2){
				
				 part_channel(sockfd,nickname,arg[1].erase(0,1));
			}
				

		}
		else if(arg[0]=="PRIVMSG"){
			if (arg.size()>=3){
				string message=arg[2].erase(0,1);
				for (int i = 3 ;i<arg.size(); i++){
					
					message =message + " "+ arg.at(i);
				}
				privmsg(sockfd,nickname,arg[1].erase(0,1),message);

			}
			
		} 
		else {
			string ERR_UNKNOWNCOMMAND=":mircd 421  "+arg[0]+" :Unknown command\r\n";
			
			write(sockfd, ERR_UNKNOWNCOMMAND .c_str(), ERR_UNKNOWNCOMMAND.length());
					
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
			set_map(tmp );
			// string motd 	= ":mircd 372 user1 :-  Hello, World!\r\n";
			// write( tmp, motd .c_str(), motd .length());
		 

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
					// remove from map
					int tmp=sockfd;
					
					erase_map(tmp );
					close(sockfd);


					client[i].fd = -1;
				}
				else{
					//printf("%s \n",buf);
					deal_input( sockfd ,buf,n );
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