//DNS Query Program on Linux
//Author : Silver Moon (m00n.silv3r@gmail.com)
//Dated : 29/4/2009

//Header Files

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<stdlib.h>    //malloc
#include<sys/socket.h>    //you know what this is for
#include<arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include<netinet/in.h>
 
#include <vector>
#include <string> 
#include "config.h"

using namespace std;
//List of DNS Servers registered on the system
char dns_servers[10][100];
int dns_server_count = 0;
//Types of DNS resource records :)

#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server
#define T_TXT 16 //TXT
#define T_AAAA 28 //IPV6
//Function Prototypes
void ngethostbyname(unsigned char*, int);
void ChangetoDnsNameFormat(unsigned char*, unsigned char*);
unsigned char* ReadName(unsigned char*, unsigned char*, int*);
void get_dns_servers(char array[] );
void  foreign( int cs,char sbuf[],int slen,struct sockaddr_in csin);
unsigned char* get_qname(char buf[],unsigned char* qname );
void send_dns(int s,  char  buf[] ,struct sockaddr_in dest,vector<vector<string>> RR_write);
//DNS header structure
struct DNS_HEADER {
	unsigned short id; // identification number

	unsigned char rd :1; // recursion desired
	unsigned char tc :1; // truncated message
	unsigned char aa :1; // authoritive answer
	unsigned char opcode :4; // purpose of message
	unsigned char qr :1; // query/response flag

	unsigned char rcode :4; // response code
	unsigned char cd :1; // checking disabled
	unsigned char ad :1; // authenticated data
	unsigned char z :1; // its z! reserved
	unsigned char ra :1; // recursion available

	unsigned short q_count; // number of question entries
	unsigned short ans_count; // number of answer entries
	unsigned short auth_count; // number of authority entries
	unsigned short add_count; // number of resource entries
};

//Constant sized fields of query structure
struct QUESTION {
	unsigned short qtype;
	unsigned short qclass;
};

//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA {
	unsigned short type;
	unsigned short _class;
 
	int ttl;
	unsigned short data_len;
};
#pragma pack(pop)

//Pointers to resource record contents
struct RES_RECORD {
	unsigned char *name;
	struct R_DATA *resource;
	unsigned char *rdata;
};

//Structure of a Query
typedef struct {
	unsigned char *name;
	struct QUESTION *ques;
} QUERY;
 
void dns_main( int s,  char  buf[],int rlen ,struct sockaddr_in dest,char * filename) {
	  

	//Get the DNS servers from the resolv.conf file
	//read  config
	unsigned char* qname = (unsigned char*) &buf[sizeof(struct DNS_HEADER)];
	struct QUESTION* qinfo = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER)
			+ (strlen((const char*) qname) + 1)];
	 
	unsigned char * name= get_qname( buf  , qname );int qtype=ntohs(qinfo->qtype);
	printf("name %s %d\n",name,qtype);

	get_total(filename);
	 printf("server %s \n",get_config_server()); 
	 get_dns_servers(get_config_server());
	
	vector<vector<string>>res=   check_in_config( name,qtype);
	if (!res.size()){
			
			 printf("not found \n");
				foreign(s,buf,rlen,dest);
		    
	}
	else{
 		send_dns( s,  buf ,dest, res);
		
	}
	 
	 
 
	 

	 
}

/*
 * Perform a DNS query by sending a packet
 * */
void Header_question_in(char buf[],char send_buf[],int& stop, char root[]){
	
 
 
	struct QUESTION *qinfo = NULL;
	 
	unsigned char*qname={};

	//Set the DNS structure to standard queries
	 
	
	 
	qname = (unsigned char*) &buf[sizeof(struct DNS_HEADER)];
	qinfo = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER)
			+ (strlen((const char*) qname) + 1)];
	
 
	
	 
 
	 
	stop =  sizeof(QUESTION)+ sizeof(struct DNS_HEADER)+strlen((const char*)qname)+1; 
	memcpy(send_buf,buf,stop);
	memcpy(root,&buf[stop],23);
	 
} 
unsigned char* get_qname(char buf[],unsigned char* qname )
{
	int what;unsigned char* name ;
	name= ReadName(qname,(unsigned char *)&buf,&what);
 
	return name;
}
void modify_dns( char send_buf[],int ans_count,int auth_count,int add_count){
	
	struct DNS_HEADER * dns=NULL;
	dns = (struct DNS_HEADER *) send_buf;
	
	 
	dns->qr=1;	dns->aa=1;	 dns->ad=0;	dns->rcode=0;

	dns->ans_count =htons(ans_count);
	dns->auth_count=htons(auth_count);
	dns->add_count =htons(add_count+ 1);
}
void RR_copy_in_(char send_buf[],int& stop,RES_RECORD* ans ){
	/*
	 copy in send
	*/
	
	 
	//name
	memcpy(&send_buf[stop],ans->name,strlen((const char*)ans ->name)+1 ); 
	stop+=strlen((const char*)ans ->name)+1 ;
	// zero
	 
	//R=DATA
	memcpy(&send_buf[stop],ans->resource,sizeof(R_DATA));
	stop+=sizeof(R_DATA) ;
 

	memcpy(&send_buf[stop], ans->rdata,ntohs(ans->resource->data_len ) );
	stop+= ntohs(ans->resource->data_len)  ;

}
void A_type_RR(char* send_buf, int &stop,unsigned char test[],string A,int ttl)
{
		//ans 
	struct RES_RECORD *ans=new RES_RECORD  ;
	
	ans->name = new unsigned char[64]  ;ans->rdata= new unsigned char[256];
	ans->resource=new R_DATA; 
	 
	//ans name
	 
	ChangetoDnsNameFormat(ans->name,test); 
	//rdata
	 
	uint32_t ip_A=0;
	inet_pton(AF_INET,A.c_str(),(void*)&ip_A);
	memcpy(ans->rdata,&ip_A,sizeof(uint32_t));
	
	
	//R-DATA-resource
	ans->resource->type =htons(T_A);
	ans->resource->_class=htons(1);
	ans->resource->ttl=   ntohl(ttl);
	ans->resource->data_len= htons(sizeof(uint32_t));
	 /*	copy into send  */
	RR_copy_in_(send_buf,stop,ans);
}
void AAAA_type_RR(char* send_buf, int &stop,unsigned char test[],string A,int ttl)
{
		//ans 
	struct RES_RECORD *ans=new RES_RECORD  ;
	
	ans->name = new unsigned char[64]  ;ans->rdata= new unsigned char[256];
	ans->resource=new R_DATA; 
	 
	//ans name
	 
	ChangetoDnsNameFormat(ans->name,test); 
	//rdata
	 
	char ip_A[16] ;
	inet_pton( AF_INET6,A.c_str(),(void*)&ip_A);
	memcpy(ans->rdata,&ip_A,16);
	
	
	//R-DATA-resource
	ans->resource->type =htons(T_AAAA);
	ans->resource->_class=htons(1);
	ans->resource->ttl=   ntohl(ttl);
	ans->resource->data_len= htons(16);
	 /*	copy into send  */
	RR_copy_in_(send_buf,stop,ans);
}

void NS_type_RR(char* send_buf,int &stop,unsigned char test[],string  MNAME ,int ttl){
		//ans 
	struct RES_RECORD *ans=new RES_RECORD  ;
	ans->resource=new R_DATA; 
	unsigned char *dns_format=new unsigned  char[64];
	ans->name = new unsigned char[64]  ;ans->rdata= new unsigned char[64];

	unsigned char NSDNAME[64] ;
	strcpy((char*)NSDNAME, MNAME.c_str()) ;
	// test[]="example1.org",A[]="140.113.80.1";
	//ans name
	ChangetoDnsNameFormat(ans->name,(unsigned char*)test); 
	//rdata
	 
 
	 
	ChangetoDnsNameFormat(ans->rdata,(unsigned char*)NSDNAME  ); 
	 
	
	//R-DATA-resource
	ans->resource->type =htons(T_NS);
	ans->resource->_class=htons(1);
	ans->resource->ttl=   ntohl(ttl);
	ans->resource->data_len= htons(strlen((const char *)ans->rdata)+1);
	 /*	copy into send  */
	RR_copy_in_(send_buf,stop,ans);
}


void CNAME_type_RR(char* send_buf,int &stop,unsigned char test[],string  MNAME ,int ttl){
		//ans 
	struct RES_RECORD *ans=new RES_RECORD  ;
	ans->resource=new R_DATA; 
	unsigned char *dns_format=new unsigned  char[64];
	ans->name = new unsigned char[64]  ;ans->rdata= new unsigned char[64];

	unsigned char NSDNAME[64] ;
	strcpy((char*)NSDNAME, MNAME.c_str()) ;
	// test[]="example1.org",A[]="140.113.80.1";
	//ans name
	ChangetoDnsNameFormat(ans->name,(unsigned char*)test); 
	//rdata
	 
 
	 
	ChangetoDnsNameFormat(ans->rdata,(unsigned char*)NSDNAME  ); 
	 
	
	//R-DATA-resource
	ans->resource->type =htons(T_CNAME);
	ans->resource->_class=htons(1);
	ans->resource->ttl=   ntohl(ttl);
	ans->resource->data_len= htons(strlen((const char *)ans->rdata)+1);
	 /*	copy into send  */
	RR_copy_in_(send_buf,stop,ans);
}
void TXT_type_RR(char* send_buf,int &stop,unsigned char test[],string  MNAME ,int ttl){
		//ans 
	struct RES_RECORD *ans=new RES_RECORD  ;
	ans->resource=new R_DATA; 
	unsigned char *dns_format=new unsigned  char[64];
	ans->name = new unsigned char[64]  ;ans->rdata= new unsigned char[64];

	unsigned char NSDNAME[64] ;
	strcpy((char*)NSDNAME, MNAME.c_str()) ;
	// test[]="example1.org",A[]="140.113.80.1";
	//ans name
	char start=(strlen((const char*)NSDNAME)+1)-'0';
	char tmp[256] ;
	tmp[1]=start;
	strcpy(tmp+1,(char*)NSDNAME);

	memcpy(ans->name,tmp,strlen(tmp)+1 );
	printf("ans ->name %s  len %ld \n",ans->name,strlen((const char*)ans->name)+1);
	//ChangetoDnsNameFormat(ans->name,(unsigned char*)test); 
	//rdata
	 
 
	 
	ChangetoDnsNameFormat(ans->rdata,(unsigned char*)NSDNAME  ); 
	 
	
	//R-DATA-resource
	ans->resource->type =htons(T_TXT);
	ans->resource->_class=htons(1);
	ans->resource->ttl=   ntohl(ttl);
	ans->resource->data_len= htons(strlen((const char *)ans->rdata)+1);
	 /*	copy into send  */
	RR_copy_in_(send_buf,stop,ans);
}

void SOA_type_RR(char* send_buf,int &stop,unsigned char test[],string SOA_data,int ttl){
		//ans 
	struct RES_RECORD *ans=new RES_RECORD  ;
	int count=0; 
	 


	vector<string >data = split(SOA_data," ");
	ans->resource=new R_DATA; 
	 
	ans->name = new unsigned char[64]  ;ans->rdata= new unsigned char[64];

	 
	//ans name
	ChangetoDnsNameFormat(ans->name,(unsigned char*)test); 
	//rdata
	struct soa_rdata {
		uint32_t SERIAL    ;  
		uint32_t REFRESH    ;
		uint32_t RETRY       ;
		uint32_t EXPIRE      ;
		uint32_t MINIMUM     ;
	};
	soa_rdata* s_rada=new soa_rdata;
	unsigned char MNAME[64] ;
	strcpy((char*)MNAME,data.at(0).c_str()) ;
	unsigned char RNAME [64];
	strcpy((char*)RNAME,data.at(1).c_str()) ;

	s_rada-> SERIAL   = ntohl (stoi(data.at(2)));
	s_rada-> REFRESH =  ntohl (stoi(data.at(3)));
	s_rada-> RETRY  =   ntohl (stoi(data.at(4)));
	s_rada-> EXPIRE =   ntohl (stoi(data.at(5)));
	s_rada-> MINIMUM =   ntohl (stoi(data.at(6)));


	ChangetoDnsNameFormat(ans->rdata,(unsigned char*) MNAME  );
	count+= strlen((const char *)ans->rdata)+1;
	ChangetoDnsNameFormat(&ans->rdata[strlen((const char *)ans->rdata)+1],(unsigned char*) RNAME  ); 
	count+= strlen((const char *)RNAME)+1;
	memcpy(&ans-> rdata[count],s_rada,sizeof(soa_rdata)); 
	count+= sizeof(soa_rdata);
	
	//R-DATA-resource
	ans->resource->type =htons(T_SOA);
	ans->resource->_class=htons(1);
	ans->resource->ttl=   ntohl(ttl);
	ans->resource->data_len= htons(count);
	 /*	copy into send  */
	RR_copy_in_(send_buf,stop,ans);
}


void MX_type_RR(char* send_buf,int &stop,unsigned char test[], string  MX_data ,int ttl){
		//ans 
	struct RES_RECORD *ans=new RES_RECORD  ;
	ans->resource=new R_DATA; 
	 
	ans->name = new unsigned char[64]  ;ans->rdata= new unsigned char[64];
	vector<string >data = split(MX_data," ");
	unsigned char NSDNAME[64] ;
	strcpy((char*)NSDNAME, data.at(1).c_str()) ;
 
	//ans name
	ChangetoDnsNameFormat(ans->name,(unsigned char*)test); 
	//rdata
	uint16_t priority = htons(stoi(data.at(0)));
	memcpy(ans->rdata,&priority,sizeof(uint16_t));
	 
	ChangetoDnsNameFormat(ans->rdata+sizeof(uint16_t),(unsigned char*)NSDNAME ); 
	 
	
	//R-DATA-resource
	ans->resource->type =htons(T_MX);
	ans->resource->_class=htons(1);
	ans->resource->ttl=   ntohl(ttl);
	ans->resource->data_len= htons(sizeof(uint16_t)+ strlen((const char *)NSDNAME)+1);
	 /*	copy into send  */
	RR_copy_in_(send_buf,stop,ans);
}
void send_dns(int s,  char  buf[] ,struct sockaddr_in dest,vector<vector<string>> RR_write){
	
	int stop=0; unsigned short qtype;
	char    root[24],send_buf[65536];root[23]='\0';
	unsigned char *name ;
	//fix question
	unsigned char* qname = (unsigned char*) &buf[sizeof(struct DNS_HEADER)];
	struct QUESTION* qinfo = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER)
			+ (strlen((const char*) qname) + 1)];
	 
	name= get_qname( buf  , qname );qtype=ntohs(qinfo->qtype);
	printf("name %s %d\n",name,qtype);
	//vector<vector<string>>res=   check_in_config( name,qtype);
	// if (!res.size()){
			
		// 	 cout<<"not found \n";
		// 		foreign(s,buf,rlen,csin);
		//    return
		// 	}

	//initial copy header and get root
	Header_question_in(  buf ,  send_buf ,  stop,   root  );
	 

	 
	//modify dns
	 
	
	//ans
		 
		switch(    qtype ) { 
        case T_A:
             
            if( RR_write.size()==1)
				{
					modify_dns(send_buf,0,1,0);
                SOA_type_RR( send_buf, stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
				} 
				else
				{
					modify_dns(send_buf,1,1,0);

                A_type_RR( send_buf,  stop,name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));

				char *tmp  =new char[64]; strcpy(tmp,(char*) name);
		
				char *p=strtok( tmp,"." );
	 			 p = strtok(NULL,  "\0");

				NS_type_RR( send_buf,  stop, (unsigned char *) p,RR_write.at(1).at(4),stoi(RR_write.at(1).at(1)));
				}
            break;
        case T_AAAA:
			if( RR_write.size()==1)
				{
					modify_dns(send_buf,0,1,0);
                SOA_type_RR( send_buf, stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
				} 
				else
				{
					modify_dns(send_buf,1,1,0);

                AAAA_type_RR( send_buf,  stop,name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));

				char *tmp  =new char[64]; strcpy(tmp,(char*) name);
		
				char *p=strtok( tmp,"." );
	 			 p = strtok(NULL,  "\0");

				NS_type_RR( send_buf,  stop, (unsigned char *) p,RR_write.at(1).at(4),stoi(RR_write.at(1).at(1)));
				}
            // isFind=find_type_name_match(check ,"AAAA",contain,res);
            // if(isFind )
            //     find_type_name_match("." ,"NS",contain,res);
            // else
            //     find_type_name_match(check ,"SOA",contain,res);
            break;
        case T_NS:
				{
					modify_dns(send_buf,1,0,1);
            // isFind=find_type_name_match(check ,"NS",contain,res);
           
				
				NS_type_RR( send_buf,  stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
               
				

            // isFind=find_type_name_match("dns" ,"A",contain,res);
				 char  tmp  [64]="dns."; strcpy(&tmp[4],(char*) name);
				A_type_RR( send_buf,  stop,(unsigned char*)tmp,RR_write.at(1).at(4),stoi(RR_write.at(1).at(1)));

				}
             break; 
        case T_SOA:{
					modify_dns(send_buf,1,1,0);
           // isFind=find_type_name_match(check ,"SOA",contain,res);
				SOA_type_RR( send_buf, stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
            // isFind=find_type_name_match("." ,"NS",contain,res);
				 
				A_type_RR( send_buf,  stop,name,RR_write.at(1).at(4),stoi(RR_write.at(1).at(1)));

				}
            
            
           

            break;      
             
        case T_MX:
		{
			  
            if( RR_write.size()==1)
				{
					modify_dns(send_buf,0,1,0);
                SOA_type_RR( send_buf, stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
				} 
			else{
				modify_dns(send_buf,1,1,1);
				 MX_type_RR( send_buf, stop,name,  RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
				

				NS_type_RR( send_buf,  stop,  name,RR_write.at(1).at(4),stoi(RR_write.at(1).at(1)));
				
				A_type_RR( send_buf,  stop,name,RR_write.at(2).at(4),stoi(RR_write.at(2).at(1)));


				}
          
			}
            break;
         
        case T_TXT:
			if( RR_write.size()==1)
					{
						modify_dns(send_buf,0,1,0);
					SOA_type_RR( send_buf, stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
					}
			else{
				modify_dns(send_buf,1,1,0);
				TXT_type_RR( send_buf,  stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
				NS_type_RR( send_buf,  stop,  name,RR_write.at(1).at(4),stoi(RR_write.at(1).at(1)));
			}
            // isFind=find_type_name_match(check ,"TXT",contain,res);
            // if(isFind )
            //     find_type_name_match("." ,"NS",contain,res);
            // else
            //     find_type_name_match(check ,"SOA",contain,res);
            break;

        case T_CNAME:
			 if( RR_write.size()==1)
				{
					modify_dns(send_buf,0,1,0);
                SOA_type_RR( send_buf, stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
				}
			else{
				modify_dns(send_buf,1,1,0);
				CNAME_type_RR( send_buf,  stop,  name,RR_write.at(0).at(4),stoi(RR_write.at(0).at(1)));
				NS_type_RR( send_buf,  stop,  name,RR_write.at(1).at(4),stoi(RR_write.at(1).at(1)));
			}
			//find_type_name_match(check ,"SOA",contain,res);
            break;
        default:
            
            break; 
    	} 



		 string ip_A="dns.demo1.org.";  string  MXNAME="mail.demo1.org.";string rdata="dns.demo1.org. admin.demo1.org. 2023010501 3600 300 3600000 3600";
		//printf("send ns %s \n",ip_A);
		//A_type_RR( send_buf,  stop,name,ip_A,3600);
		//SOA_type_RR( send_buf, stop,  name,rdata,3600);
		// MX_type_RR( send_buf, stop,name,10 ,  MXNAME ,3600);
		// NS_type_RR( send_buf,  stop,name,ip_A,3600);
	
	//set root

	
	memcpy(&send_buf[stop],root,23);
	stop+=23;
	//send back
	printf("\nSending Packet...");
	if (sendto(s, (char*) send_buf,
			stop, 0, (struct sockaddr*) &dest,
			sizeof(dest)) < 0) {
		perror("sendto failed");
	}
	printf("Done");
}




void  foreign( int cs,char sbuf[],int slen,struct sockaddr_in csin) {
 
	char buf[65536]={};struct sockaddr_in dest;

	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP packet for DNS queries
	// char array[]="208.67.222.222";
	// get_dns_servers(array);
	dest.sin_family = AF_INET;
	dest.sin_port = htons(53);
	dest.sin_addr.s_addr = inet_addr(dns_servers[0]); //dns servers

 

	printf("\nSending Packet...");
	if (sendto(s, (char*) sbuf,
			slen, 0, (struct sockaddr*) &dest,
			sizeof(dest)) < 0) {
		perror("sendto failed");
	}
	printf("Done");

	//Receive the answer
	int i = sizeof (dest);
	int rlen=0;
	printf("\nReceiving answer...");
	if ( (rlen=recvfrom(s, (char*) buf, 65536, 0, (struct sockaddr*) &dest,
			(socklen_t*) &i)) < 0) {
		perror("recvfrom failed");
	}
	printf("Done");
	
	printf("\nSending Back...");
	sendto(cs, buf, rlen, 0, (struct sockaddr*) &csin, sizeof(csin));
		 
	printf("Done");
 
}


//slice question


/*
 *
 * */
unsigned  char* ReadName(unsigned char* reader, unsigned char* buffer, int* count) {
	unsigned char *name;
	unsigned int p = 0, jumped = 0, offset;
	int i, j;

	*count = 1;
	name = (unsigned char*) malloc(256);

	name[0] = '\0';

	//read the names in 3www6google3com format
	while (*reader != 0) {
		if (*reader >= 192) {
			offset = (*reader) * 256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
			reader = buffer + offset - 1;
			jumped = 1; //we have jumped to another location so counting wont go up!
		} else {
			name[p++] = *reader;
		}

		reader = reader + 1;

		if (jumped == 0) {
			*count = *count + 1; //if we havent jumped to another location then we can count up
		}
	}

	name[p] = '\0'; //string complete
	if (jumped == 1) {
		*count = *count + 1; //number of steps we actually moved forward in the packet
	}

	//now convert 3www6google3com0 to www.google.com
	for (i = 0; i < (int) strlen((const char*) name); i++) {
		p = name[i];
		for (j = 0; j < (int) p; j++) {
			name[i] = name[i + 1];
			i = i + 1;
		}
		name[i] = '.';
	}
	//name[i - 1] = '\0'; //remove the last dot
	return name;
}

/*
 * Get the DNS servers from /etc/resolv.conf file on Linux
 * */
void get_dns_servers(char array[]) {
	 

	strcpy(dns_servers[0], array);
	printf("dns server %s\n ",dns_servers[0]);
	// strcpy(dns_servers[1], "208.67.220.220");
}

/*
 * This will convert www.google.com to 3www6google3com
 * got it :)
 * */
void ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host) {
	int lock = 0, i;
	char* tmp = new  char[64] ;
	strcpy(tmp,(const char*) host);
	strcat((char*) tmp, ".");

	for (i = 0; i < strlen((char*) tmp); i++) {
		if (tmp[i] == '.') {
			*dns++ = i - lock;
			for (; lock < i; lock++) {
				*dns++ = tmp[lock];
			}
			lock++; //or lock=i+1;
		}
	}
	*dns++ = '\0';
	 
	 
}