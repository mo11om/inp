/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define err_quit(m) { perror(m); exit(-1); }
#define CHUNKSIZE 1000

static unsigned count = 0;
static int totalfiles = 0;
static char fpath[16];
static int currfile = -1;
static int fsize = -1;
static int fchunks[1000][35];

void store_file(char *name, char *chunkstr, int total, int size, char *buf, FILE *fptr) {
	char fname[24];
	int fnum = atoi(name);
	int chunk = atoi(chunkstr);
	if(fchunks[fnum][0] < 0) return;
	sprintf(fname, "%s/%s", fpath, name);
	// printf("fname: %s, currfile: %d, chunk: %s\n", name, currfile, chunkstr);
	if(fnum != currfile) { // it's a new file
		if(currfile >= 0) fchunks[currfile][0] = -1;
		if(fptr != NULL) {
			fclose(fptr);
			printf("close file: %d\n", currfile);
		}
		fptr = fopen(fname, "r+");
		fchunks[fnum][0] = total;
		currfile = fnum;
	}
	if(fchunks[fnum][1+chunk] > 0) return;
	fchunks[fnum][1+chunk] = 1;
	// printf("fchunks: %d, chunk: %d\n", fchunks[fnum], chunk);
	fptr = fopen(fname, "r+");
	
	if(fptr == NULL) puts("open fptr NULL!");
	fseek(fptr, CHUNKSIZE*chunk, SEEK_SET);
	fwrite(buf, 1, size, fptr); 
	fclose(fptr);
	if(fchunks[fnum][0] < 0) count++;
}

// /server <path-to-store-files> <total-number-of-files> <port>
int main(int argc, char *argv[]) {
	int s;
	struct sockaddr_in sin;

	if(argc < 4) {
		return -fprintf(stderr, "usage: %s ... <path-to-store-files> <total-number-of-files> <port>\n", argv[0]);
	}

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(strtol(argv[argc-1], NULL, 0));

	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");

	if(bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0)
		err_quit("bind");

    totalfiles = strtol(argv[2], NULL, 0);
    memset(fpath, 0, 16);
    sprintf(fpath, "%s", argv[1]);
    printf("total: %d, path: %s\n", totalfiles, fpath);

	char fname[36];
	FILE *fptr;
	for(int i = 0; i < totalfiles; i++) {
		memset(fname, 0, 36);
		sprintf(fname, "%s/%06d", fpath, i);
		fptr = fopen(fname, "w"); //打開檔案
		fclose(fptr);
		fchunks[i][0] = 0;
	}
	fptr = NULL;
	struct sockaddr_in csin;
	socklen_t csinlen = sizeof(csin);
	char buf[CHUNKSIZE+24];
	int rlen;
	char rname[10];
	char rchunk[4];
	char rtotal[4];
	char rcsize[8];
	struct timespec ts0;
	ts0.tv_sec = 0;
	ts0.tv_nsec = 100;
	int size, total;
	while(1) {
        if(count == totalfiles) break;
		
		memset(buf, 0, CHUNKSIZE+24);
		if((rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &csin, &csinlen)) < 0) {
			perror("recvfrom");
			break;
		}
		memset(rname, 0, 10);
		memset(rchunk, 0, 4);
		memset(rtotal, 0, 4);
		memset(rcsize, 0, 8);
		// printf("rlen: %d, buflen: %ld\n", rlen, strlen(buf));
		if(rlen < 14) continue;
		
		//要傳送給client的
		//格式: [檔名(6B)][區塊號碼(2B)][總區塊數(2B)][區塊大小(4B)] -----共14B要傳
		strncpy(rname, buf, 6);
		strncpy(rchunk, buf+6, 3);
		strncpy(rtotal, buf+9, 2);
		strncpy(rcsize, buf+11, 4);
		int total = atoi(rtotal);
		int size = atoi(rcsize);

		// puts("\n--------for client------");
		// printf("file name: %s\n", rname);
		// printf("chunk/total: %s/%s\n", rchunk, rtotal);
		// printf("file size: %s\n", rcsize);
		// printf("content: %s\n", buf+15);
		// puts("--------end client-------\n");
        sendto(s, buf, 15, 0, (struct sockaddr*) &csin, sizeof(csin));
        store_file(rname, rchunk, total, size, buf+15, fptr);
		nanosleep(&ts0, NULL);
	}

	close(s);
}
