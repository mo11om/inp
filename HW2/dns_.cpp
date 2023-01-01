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
#include<unistd.h>    //getpid
 
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

//Function Prototypes
void ngethostbyname(unsigned char*, int);
void ChangetoDnsNameFormat(unsigned char*, unsigned char*);
unsigned char* ReadName(unsigned char*, unsigned char*, int*);
void get_dns_servers(char array[] );

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
 
// int main(int argc, char *argv[]) {
// 	unsigned char hostname[100];

// 	//Get the DNS servers from the resolv.conf file
// 	get_dns_servers();

// 	//Get the hostname from the terminal
// 	printf("Enter Hostname to Lookup : ");
// 	scanf("%s", hostname);

// 	//Now get the ip of this hostname , A record
// 	ngethostbyname(hostname, T_A);

// 	return 0;
// }

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
void A_type_RR(char* send_buf, int &stop,unsigned char test[],unsigned char A[],int ttl){
		//ans 
	struct RES_RECORD *ans=new RES_RECORD  ;
	ans->resource=new R_DATA; 
	unsigned char *dns_format=new unsigned  char[64]; 
	// test[]="example1.org",A[]="140.113.80.1";
	//ans name
	ans->name = new unsigned char[64]  ;ans->rdata= new unsigned char[64];
	ChangetoDnsNameFormat(ans->name,(unsigned char*)test); 
	//rdata
	uint32_t ip_A=0;
	inet_pton(AF_INET,"140.113.80.1",(void*)&ip_A);
	memcpy(ans->rdata,&ip_A,sizeof(uint32_t));
	
	
	//R-DATA-resource
	ans->resource->type =htons(T_A);
	ans->resource->_class=htons(1);
	ans->resource->ttl=   ntohl(ttl);
	ans->resource->data_len= htons(sizeof(uint32_t));
	 /*	copy into send  */
	RR_copy_in_(send_buf,stop,ans);
}

void send_dns(int s,  char  buf[] ,struct sockaddr_in dest){
	
	int stop=0; unsigned short qtype;
	char    root[24],send_buf[65536];root[23]='\0';
	unsigned char *name ;
	//fix question
	unsigned char* qname = (unsigned char*) &buf[sizeof(struct DNS_HEADER)];
	struct QUESTION* qinfo = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER)
			+ (strlen((const char*) qname) + 1)];
	
	//initial copy header and get root
	Header_question_in(  buf ,  send_buf ,  stop,   root  );
	 

	 
	//modify dns
	 
	modify_dns(send_buf,1,0,0);
		//ans
		unsigned  char  rr_name[]="example1.org",ip_A[]="140.113.80.1";
		A_type_RR( send_buf,  stop,rr_name,ip_A,3600);
	
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
	char array[]="208.67.222.222";
	get_dns_servers(array);
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
	// strcpy(dns_servers[1], "208.67.220.220");
}

/*
 * This will convert www.google.com to 3www6google3com
 * got it :)
 * */
void ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host) {
	int lock = 0, i;
	strcat((char*) host, ".");

	for (i = 0; i < strlen((char*) host); i++) {
		if (host[i] == '.') {
			*dns++ = i - lock;
			for (; lock < i; lock++) {
				*dns++ = host[lock];
			}
			lock++; //or lock=i+1;
		}
	}
	*dns++ = '\0';
	 
	 
}