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
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#define MAX 65536
using namespace std;

 


void  own_dns(unsigned  char qname[]) {
   
     vector <string> dns_name;
    const char d  ='\x03' ; 
    char dest[50];
     
 
    memcpy(dest, qname, strlen((const char*)qname)+1);
 
     
    char *p;
    p = strtok(dest, &d);
    
    while (p != NULL) {
        //printf("%s\n", p);
        string tmp=p;
        dns_name.push_back(tmp);
        p = strtok(NULL, &d);		   
    }
    printf("\ndns name %ld",dns_name.size());
    
    for(int i=0 ;i<dns_name.size();i++)cout<<endl<<i<<dns_name.at(i); 

      
    
}
