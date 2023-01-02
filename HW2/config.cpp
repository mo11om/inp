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
#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server
#define T_TXT 16 //Mail server
#define T_AAAA 28 //Mail server
using namespace std;

 
//define  all store zone
map<string, vector<vector <string>>>all_dns_record;
string dns_server;
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
    //for(int i =0;i<args.size();i++)cout<<i<<(args.at(i))<<" ";
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
void print_ALL(vector<vector <string>>&Zone){
    for(int i =0  ;i<Zone.size();i++){
        
        for (int j =0  ;j<Zone.at(i).size();j++){
            if(Zone.at(i).at(j)=="@")Zone.at(i).at(j)=".";
            cout<< Zone.at(i).at(j)<<" ";

        }
            
            cout<<endl;
    }
}
void get_total(char* file_name ){
    vector<vector <string>> Config ;
    vector<vector <string>>Zone;
    Config=get_file_split_res(file_name);

     dns_server= Config[0][0];
 
    print_ALL(Config);
    for(int i =1  ;i<Config.size();i++){
        
         
        vector<vector<string>> zone_record= (get_file_split_res(Config.at(i).at(1).c_str() ));
        vector<vector<string>>::const_iterator first = zone_record.begin() + 1 ;
        vector<vector<string>>::const_iterator last = zone_record.end()  ;
        vector<vector<string>> record(first, last);
        
      
        all_dns_record[Config.at(i).at(0)]=record;
        cout<<"\n  \n";cout<<Config.at(i).at(0)<<endl;
        print_ALL(all_dns_record[ Config.at(i).at(0)]);
    }


    
    
}

bool find_type_name_match(string name,string type, vector<vector<string>>& contain,vector<vector<string>>& res)
{
    for (int i =0;i<contain.size();i++){
         cout<<type <<" "<<contain.at(i).at(3)<<" " << contain.at(i).at(0)<<endl;
        if (type==contain.at(i).at(3))
            if (name.find(contain.at(i).at(0)) != std::string::npos) 
                {res.push_back(contain.at(i));
                    return true;
                }

    }
    return false;
}
void get_need_vec(unsigned short qtype,string  check , vector<vector<string>>& contain,vector<vector<string>>& res )
{
     
    bool isFind=false;
     switch(    qtype ) { 
        case T_A:
            isFind=find_type_name_match(check ,"A",contain,res);
            if(isFind )
                find_type_name_match("." ,"NS",contain,res);
            else
                find_type_name_match(check ,"SOA",contain,res);
            break;
        case T_AAAA:
            isFind=find_type_name_match(check ,"AAAA",contain,res);
            if(isFind )
                find_type_name_match("." ,"NS",contain,res);
            else
                find_type_name_match(check ,"SOA",contain,res);
            break;
        case T_NS:
            isFind=find_type_name_match(check ,"NS",contain,res);
            
            isFind=find_type_name_match("dns" ,"A",contain,res);

            break; 
        case T_SOA:
            isFind=find_type_name_match(check ,"SOA",contain,res);
            
            isFind=find_type_name_match("." ,"NS",contain,res);

            break;      
             
        case T_MX:
            isFind=find_type_name_match(check ,"MX",contain,res);
            if(isFind ){
                find_type_name_match("." ,"NS",contain,res);
                find_type_name_match("mail","A",contain,res);
                }
            else
                find_type_name_match(check ,"SOA",contain,res);

            break;
         
        case T_TXT:
            isFind=find_type_name_match(check ,"TXT",contain,res);
            if(isFind )
                find_type_name_match("." ,"NS",contain,res);
            else
                find_type_name_match(check ,"SOA",contain,res);
            break;

        case T_CNAME:
            isFind=find_type_name_match(check ,"CNAME",contain,res);
            if(isFind )
                find_type_name_match("." ,"NS",contain,res);
            else
                find_type_name_match(check ,"SOA",contain,res);
            break;
             
        default:
            
            break; 
    } 
}




vector<vector<string>> check_in_config(unsigned char name[],unsigned short qtype){
    string  check ( reinterpret_cast< char const* >(name) ) ;  
     vector<vector <string>>  found;
    cout<<"check "<< name<<endl ;cout<<qtype<<endl;
    map<string, vector<vector <string>>>::iterator it;
    for (it=all_dns_record.begin(); it!=all_dns_record.end();it++){
        if (check.find(it->first) != std::string::npos) {
                std::cout << "found!" << '\n';
                get_need_vec(qtype, check ,it->second,    found);
                print_ALL(found);
                return found;
            }
    }

    return   found;
}

 char * get_config_server(){

    char* arr= new char[256]; 
    strcpy(arr, dns_server.c_str() ); 
    return arr;
}

void check_SoA(vector<string>& soa){
    vector<string> rdata= split( soa.at(4)," ");
     for(int i =0;i<rdata.size();i++)cout<<i<<" "<<(rdata.at(i))<<" ";
    cout <<"\n";
    // for(int i= 0 ; i<strlen(test)+1 ;i++) printf("%d %c\n",i,test[i]);
}
// int main(int argc, char *argv[]) {
//     unsigned char test[]= "demo1.org.";
//      get_total(argv[argc-1]);
//     vector<vector<string>>res=   check_in_config(test,T_A);
//     check_SoA(res.at(0));
    
// }
