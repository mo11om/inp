/*
用raw socket,
選一個不常用的byte
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>

#define err_quit(m) { perror(m); exit(-1); }

//convert addr
#define NIPQUAD(s)	((unsigned char *) &s)[0], \
					((unsigned char *) &s)[1], \
					((unsigned char *) &s)[2], \
					((unsigned char *) &s)[3]

#define MAXSTACK 1000
#define MAXCHUNK 2000
#define CHUNKSIZE 1000
static int s = -1; //socket
static int s1 = -1;
static struct sockaddr_in sin;
static unsigned seq;
static unsigned count = 0; 
static int totalfiles = 0;
//static char fbuf[1024];
static char fpath[8];
static char fname[20];
static char content[35000];

static int top = -1;
static int currfile = -1;
//static int files[1000]; //no need to send: -1, sent: 0, priority: 3(<=13000), 2(<=26000), 1(<=39000)
static int pri1[1000];
static int pri2[1000];
static int pri3[1000];
static int pri4[1000];
static int pri5[1000];

static int ppri1 = -1;
static int ppri2 = -1;
static int ppri3 = -1;
static int ppri4 = -1;
static int ppri5 = -1;

static int prinow = -1;

static int _file = 0;
static int fsize[1000];
static int fchunks[1000][35];
struct timespec ts0, ts1;
double tv2ms(struct timeval *tv) {
	return 1000.0*tv->tv_sec + 0.001*tv->tv_usec;
}

void chunkToBuf(int chunk, char *name, int _fname, char *allbuf) {
	char tmpstr[8];
	int len, pos;
	char fbuf[CHUNKSIZE+1];
	pos = CHUNKSIZE*chunk;
	if((len = strlen(content+pos)) >= CHUNKSIZE) len = CHUNKSIZE;
	memset(fbuf, 0, CHUNKSIZE+1);
	strncpy(fbuf, content+pos, len); // content
	strcpy(allbuf, name); // file name (6 bytes)，把檔名放入buffer
	memset(tmpstr, 0, 8);
	sprintf(tmpstr, "%03d", chunk); 
	strcat(allbuf, tmpstr); // chunk num (2 bytes)，把區塊號碼放入buffer，從0開始編號
	memset(tmpstr, 0, 8);
	sprintf(tmpstr, "%02d", fchunks[_fname][0]);
	strcat(allbuf, tmpstr); // total chunks (2 bytes)，把共有多少區塊放入buffer，數字至少為1
	memset(tmpstr, 0, 8);
	sprintf(tmpstr, "%04d", len); 
	strcat(allbuf, tmpstr); // file size (4 bytes)，把此區塊的長度放入buffer，應該會<=CHUNKSIZE
	strcat(allbuf, fbuf);
}

void do_send(int sig) {
sendstart:
	char fbufs[10][CHUNKSIZE+24];

    char name[8];
	int _fname;
	for(int i = 0; i < 10; i++) memset(fbufs[i], 0, CHUNKSIZE+24);

	memset(fname, 0, 20);
	memset(name, 0, 6);
	if(ppri5 >= 0) {
		prinow = 5;
		_fname = pri5[ppri5];
	}
	else if(ppri4 >= 0) {
		prinow = 4;
		_fname = pri4[ppri4];
	}
	else if(ppri3 >= 0) {
		prinow = 3;
		_fname = pri3[ppri3];
	}
	else if(ppri2 >= 0) {
		prinow = 2;
		_fname = pri2[ppri2];
	}
	else if(ppri1 >= 0) {
		prinow = 1;
		_fname = pri1[ppri1];
	}
	else exit(0);
	// printf("errno: %d, errMsg: %s\n", errno, strerror(errno));
	// printf("<_fname>: %d\n", _fname);
	sprintf(fname, "%s/%06d", fpath, _fname);
	sprintf(name, "%06d", _fname);

	if(_fname != currfile) {//如果這個檔案不是正在處裡的file，代表是新讀的檔案，要把所有chunk放進stack中
		FILE *fptr = fopen(fname, "r"); //打開檔案
		printf("received: %d\n", currfile);
		memset(content, 0, 35000);
		fseek(fptr, 0L, SEEK_SET);
		int x = fread(content, sizeof(char), 35000, fptr);
		fclose(fptr);
		currfile = _fname;
	}
	int chunks[10];
	for(int i = 0; i < 10; i++) chunks[i] = -1;
	int i = 0, j = 1;
	while(i < 10 && j < 35) {
		if(fchunks[_fname][j] < 0) {
			chunks[i] = j - 1;
			i++;
		}
		j++;
	}
	// printf("pop chunk int: %d\n", chunk);
	if(chunks[0] < 0) {
		if(prinow == 1) ppri1--;
		else if(prinow == 2) ppri2--;
		else if(prinow == 3) ppri3--;
		else if(prinow == 4) ppri4--;
		else if(prinow == 5) ppri5--;
		else return;
		goto sendstart; //現在的檔案送完了，回去處理別的還沒送完的檔案
	}
	for(int i = 0; i < 10; i++) {
		if(chunks[i] >= 0) {
			chunkToBuf(chunks[i], name, _fname, fbufs[i]);
		}
	}
	if(sig == SIGALRM) {
		for(int i = 0; i < 10; i++) {
			if(chunks[i] < 0) return;
 			if(sendto(s, fbufs[i], CHUNKSIZE+24, 0, (struct sockaddr*) &sin, sizeof(sin)) < 0)
				perror("sendto");
			nanosleep(&ts1, NULL);
		}
	}

}

// client <path-to-read-files> <total-number-of-files> <port> <server-ip-address>
int main(int argc, char *argv[]) {
	if(argc < 5) {
		return -fprintf(stderr, "usage: %s ... <path-to-read-files> <total-number-of-files> <port> <server-ip-address>\n", argv[0]);
	}
	

	srand(time(0) ^ getpid());
    // no buf
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(strtol(argv[argc-2], NULL, 0));
	if(inet_pton(AF_INET, argv[argc-1], &sin.sin_addr) != 1) {
		return -fprintf(stderr, "** cannot convert IPv4 address for %s\n", argv[1]);
	}

	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");
	if((s1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");
    totalfiles = strtol(argv[2], NULL, 0);
	memset(fpath, 0, 8);
    sprintf(fpath, "%s", argv[1]);
	for(int i = 0; i < totalfiles; i++){
		memset(fname, 0, 20);
		sprintf(fname, "%s/%06d", fpath, i);
		FILE *fptr = fopen(fname, "r"); //打開檔案
		fseek(fptr, 0L, SEEK_END);
		fsize[i] = ftell(fptr);
		fchunks[i][0] = 1 + ((fsize[i] - 1) / CHUNKSIZE);
		for(int j = 1; j <= fchunks[i][0]; j++) fchunks[i][j] = -1;
		for(int j = fchunks[i][0]+1; j < 35; j++) fchunks[i][j] = 1;
		fclose(fptr);
		if(fsize[i] < CHUNKSIZE * 5) pri5[++ppri5] = i;
		else if(fsize[i] >= CHUNKSIZE * 5 && fsize[i] < CHUNKSIZE * 10) pri4[++ppri4] = i;
		else if(fsize[i] >= CHUNKSIZE * 10 && fsize[i] < CHUNKSIZE * 15) pri3[++ppri3] = i;
		else if(fsize[i] >= CHUNKSIZE * 15 && fsize[i] < CHUNKSIZE * 20) pri2[++ppri2] = i;
		else pri1[++ppri1] = i;
	}
	sleep(2);
    

	struct timeval tv;
	struct timeval tv_interval = {0, 12000};
    struct timeval tv_value = {4, 0};
    struct itimerval it;
    it.it_interval = tv_interval;
    it.it_value = tv_value;
	ts0.tv_sec = 0;
	ts0.tv_nsec = 10;
	ts1.tv_sec = 0;
	ts1.tv_nsec = 100000;
	int rlen;
	struct sockaddr_in csin;
	socklen_t csinlen = sizeof(csin);
	char buf[20];
	char rname[8];
	char rchunk[4];
	char rtotal[4];
	char rcsize[8];
	int srvrecv = -1;
	int srvrch = -10;
	// pthread_t tid;
	signal(SIGALRM, do_send);
	do_send(SIGALRM);
	setitimer(ITIMER_REAL, &it, NULL);

	while(1) {
		memset(buf, 0, 20);
		if((rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &csin, &csinlen)) < 0) {
			perror("recvfrom");
			continue;
		}
		memset(rname, 0, 8);
		memset(rchunk, 0, 4);
		memset(rtotal, 0, 4);
		memset(rcsize, 0, 8);
		strncpy(rname, buf, 6);
		strncpy(rchunk, buf+6, 3);
		strncpy(rtotal, buf+9, 2);
		strncpy(rcsize, buf+11, 4);
		srvrecv = atoi(rname);
		srvrch = atoi(rchunk);
		if(srvrecv == currfile && fchunks[srvrecv][srvrch+1] == -1) {
			fchunks[srvrecv][srvrch+1] = 1;
		}
			
		
		gettimeofday(&tv, NULL);
		// nanosleep(&ts0, NULL);
	}

	close(s);
}
