#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <iostream>  
 
#define MAXDATA   1024*33
#define MAXNAME   1024*33
 
#define err_quit(m) { perror(m); exit(-1); }
#define NIPQUAD(s)	((unsigned char *) &s)[0], \
					((unsigned char *) &s)[1], \
					((unsigned char *) &s)[2], \
					((unsigned char *) &s)[3] 
using namespace std;


typedef struct {
	unsigned seq;
	struct timeval tv;
}	ping_t;


string  _get_real_path_file
(string path,int file_num ){
        char tmp[30];
        sprintf(tmp,"%s/%06d",path.c_str(),file_num );
        printf ("tmp %s\n",tmp);
        string r_str=tmp;
        return r_str;
        
}

void _send_file(int fd, string path,int file_num,struct sockaddr_in servaddr){
        int fdesc;    /* file description */
        int ndata;    /* the number of file data */
        char data[MAXDATA]={'\0'}; /* read data form file */
        char data1[MAXNAME]={};  /*server response a string */
        char buf[MAXNAME];     /* holds message from server */

        socklen_t size;    /* the length of servaddr */
        
        struct timeval tv = {
        .tv_usec = 1 
        };

        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        string real_path=_get_real_path_file(path,file_num); //"./sendto/000001";
        // /**開起檔案讀取文字 **/
        
        
        fdesc = open(real_path.c_str(), O_RDONLY);
    

        if (fdesc == -1) {
                perror("open file error!");
                exit (1);
        }
        ndata = read (fdesc, data, MAXDATA);
        if (ndata < 0) {
                perror("read file error !");
                exit (1);
        }
        data[ndata + 1] = '\0'; 
        
        




        string send_data =  data; 
        
        
        char file_name[8] 
        sprintf(file_name,"%06d",file_num); 
       
        string ack ="";
        ack=(file_name ) ;
       
        // ack +=to_string(ndata)+" " ;
        printf("ack %s  %s \n",ack.c_str(),send_length );
       
        send_data=ack +send_data;
        cout<<"total send " <<to_string(send_data.length()+6)<<endl;


        size = sizeof(servaddr);


        if (write(fd, send_data.c_str() ,send_data.length()) <0) err_quit("send")

        // for (int time=0;time<15;time++){
        //  /* 發送資料給 Server */
        //         if (sendto(fd, send_data.c_str() ,send_data.length(), 0, (struct sockaddr*)&servaddr, size) == -1) {
        //                 perror("write to server error !");
        //                 exit(1);
        //         }
        //         /** 由伺服器接收回應 **/
        //         if (recvfrom(fd, data1, MAXDATA, 0, (struct sockaddr*)&servaddr,  &size) < 0) {
        //                 perror ("read from server error !");
        //                 continue;
        //         }
        //         if (data1){
                        
        //                 string rec_msg= data1;
        //                                 string receive=rec_msg.substr(0,7);
        //                                 string data_size =rec_msg.substr(7);
        //                                 int real_date_size= stoi(data_size);
        //                         if ( (receive== "receive")  )
        //                         { 
        //                                 if(real_date_size == ndata ){
        //                                        printf("%s %d\n", data1,ndata);
                                        
        //                                         break; 
        //                                 }
                                                
                                        
        //                         }
        //                 }
        // }
        
        /* 印出 server 回應 **/
         
        
        
         
}

int main(int argc, char **argv){
        int fd;     /* fd into transport provider */
        int i;     /* loops through user name */
        int length;    /* length of message */
        int on=1;
        struct sockaddr_in myaddr;   /* address that client uses */
        struct sockaddr_in servaddr; /* the server's full addr */
        
        int file_num= atoi(argv[argc-2]);
               /*
         * Check for proper usage.
         */
        if (argc < 3) {
                fprintf (stderr, "Usage: %s host_name(IP address) file_name\n", argv[0]);
                exit(2);
        }
        /*
         *  Get a socket into UDP
         */
        if ((fd = socket(AF_INET,SOCK_RAW,161)) < 0) {
                perror ("socket failed!");
                exit(1);
        }
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));


        /*
         * Bind to an arbitrary return address.
         */
        bzero((char *)&myaddr, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(argv[argc-1]);
        if(connect(s, (struct sockaddr*) &myaddr, sizeof(myaddr)) < 0) err_quit("c");
	
	
        /*
         * Fill in the server's UDP/IP address
         */
      
        // bzero((char *)&servaddr, sizeof(servaddr));
        // servaddr.sin_family = AF_INET;
       
        // inet_pton(AF_INET, argv[argc-1], &(servaddr.sin_addr));

        // servaddr.sin_addr.s_addr = inet_addr(argv[4]);
        
        // hp = gethostbyname_r(argv[4]);

        // if (hp == 0) {
        //         fprintf(stderr, "could not obtain address of %s\n", argv[4]);
        //         return (-1);
        // }
        // bcopy(hp->h_addr_list[0], (caddr_t)&servaddr.sin_addr,
        //                 hp->h_length);
        
        
        
        printf("start to send 1\n");
        string source=argv[1];
         
       
        for (int i =0 ;i<1000 ;i++)    _send_file(fd,source,i,servaddr);
        printf("end to send 1\n");
       
        close(fd);
        

}