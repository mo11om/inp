/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string> 
using namespace std;
int main(int argc, char *argv[]) {
    string tmp = argv[1];
    string data= argv[2];
	int fd =open(tmp.c_str(), O_WRONLY  |O_TRUNC  );
				if(fd==-1){
					perror("open not exist");
					fd =creat(tmp.c_str(), 0777) ;
					 
					
				}
			 
				if(( write(fd ,data.c_str() ,data.length()))==-1)perror("not write");
	close(fd);
}
