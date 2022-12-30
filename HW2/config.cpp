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
#include <cstring>
#include <iostream>
//data structure
#include <vector>
#include <map>

 

// #include "dns.h"
#define MAX 65536
using namespace std;

 
//define  all store zone
map<string, vector<vector <string>>>all_dns_record;
// vector<map<string,vector <string>>> zone_list;

const std::vector<std::string> split(const std::string& str, const std::string& pattern) {
    std::vector<std::string> result;
    std::string::size_type begin, end;

    end = str.find(pattern);
    begin = 0;

    while (end != std::string::npos) {
        if (end - begin != 0) {
            result.push_back(str.substr(begin, end-begin)); 
        }    
        begin = end + pattern.size();
        end = str.find(pattern, begin);
    }

    if (begin != str.length()) {
        result.push_back(str.substr(begin));
    }
    return result;        
}

vector<string> get_every_line(string line){
    string pattern = ",";
    vector<string> args= split(line, pattern);
    // cout<< " arg "<< line<<endl;
    // for(int i =0;i<args.size();i++)cout<<i<<(args.at(i))<<" ";
    // printf("\n");
     
    return args;
    
}
vector<vector<string>> get_file_split_res(const char* file_name ){
    char buf[MAX]={'\0'};

    int fd=0;
     
    vector<vector<string>> res;

    if((fd=open(file_name,O_RDONLY))<0)perror("zone open:");
    if(read(fd,buf,MAX)<0)perror("zone read :");

    string zone_all =buf;string pattern = "\r\n";
   // cout<<zone_all<<endl;
    vector<string> lines= split(zone_all, pattern);
     
    //cout<<lines.at(0)<<endl;
 

    for(int i =0;i<lines.size();i++) {
        // cout<<lines.at(i)<<endl;
        vector<string> tmp ;
        tmp.clear();
        tmp = ( get_every_line(lines.at(i)));
    
        //for(int i =0;i<tmp.size();i++)cout <<(tmp.at(i))<<"  ";
            
            res.push_back(tmp);
        }
     
    return res;
    
}
void print_ALL(vector<vector <string>>Zone){
    for(int i =0  ;i<Zone.size();i++){
        
        for (int j =0  ;j<Zone.at(i).size();j++)
            cout<< Zone.at(i).at(j)<<" ";
            cout<<endl;
    }
}
void get_total(char* file_name ){
    vector<vector <string>> Config ;
    vector<vector <string>>Zone;
    Config=get_file_split_res(file_name);

     

 
    print_ALL(Config);
    for(int i =1  ;i<Config.size();i++){
        
         
        vector<vector<string>> zone_record= (get_file_split_res(Config.at(i).at(1).c_str() ));
        vector<vector<string>>::const_iterator first = zone_record.begin() + 1 ;
        vector<vector<string>>::const_iterator last = zone_record.end()  ;
        vector<vector<string>> record(first, last);
        
      
        all_dns_record[Config.at(i).at(0)]=record;
        cout<<"\n new \n";cout<<Config.at(i).at(0)<<endl;
        print_ALL(all_dns_record[Config.at(i).at(0)]);
    }


    
    
}



int main(int argc, char *argv[]) {
   
    get_total(argv[argc-1]);
}
