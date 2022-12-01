#include<stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
using namespace std;
char *
gen_secret(int times ,int oseed) {
	static int len;
	static char buf[64];
	
    srand(oseed);
	for (int i = 0 ;i<times;i++) rand();
	 

	
	len  = snprintf(buf,     10, "%x", rand());
	len += snprintf(buf+len, 10, "%x", rand());
	len += snprintf(buf+len, 10, "%x", rand());
	len += snprintf(buf+len, 10, "%x", rand());
	buf[len] = '\0';
	 

	 
	return buf;
}
int main (){
    int oseed; char *buf;
	int times =20;
	 
    scanf("%x", &oseed); 
    printf("%x\n", oseed);
	for (int i = 0 ;i<times;i++){
		buf=gen_secret(i,oseed);
		 
		printf("%d %s\n",i,buf);
	}
	
    
    return 0;
}
 
//5