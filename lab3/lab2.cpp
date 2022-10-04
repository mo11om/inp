#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;
typedef struct {
        uint32_t magic;
        int32_t  off_str;
        int32_t  off_dat;
        uint32_t n_files;
} __attribute((packed)) pako_header_t;
typedef struct
{
  int32_t off_name;
  //  #if __BYTE_ORDER == __LITTLE_ENDIAN

  //  #if __BYTE_ORDER == __LITTLE_ENDIAN

  //  #endif
  uint32_t size_file;
  int32_t off_cont;

  uint64_t check_sum;

} __attribute((packed)) FILE_E;
 
int main (int argc, char *argv[]){

  int fd =open(argv[1],O_RDONLY);
  if (fd == -1 ){
    cout<< "fd "<< fd<<endl;
    return 0;
  }
  pako_header_t* pako= new pako_header_t;

  ssize_t res = read(fd,pako,sizeof(pako_header_t));

  cout<<"res : " << res<<endl;
  printf ("magic %x \n nfile %d \n offdat %x \n offstr %x\n",   (pako->magic),pako->n_files, pako->off_dat,pako->off_str);
  int32_t n_files=pako->n_files;

   FILE_E * file_pointer[n_files];
   for (size_t i = 0; i < n_files; i++){
      file_pointer[i]=new FILE_E;
   }
  for (size_t i = 0; i < n_files; i++)
  {

    read(fd,file_pointer[i],sizeof(FILE_E));
    file_pointer[i]->size_file = __builtin_bswap32 ( file_pointer[i]->size_file );
    file_pointer[i]->check_sum =  __builtin_bswap64 (  file_pointer[i]->check_sum);
    //cout<< i<<"\t"<<res<<endl;

    /* code */
  }
  // for (size_t i = 0; i <n_files; i++)
  // {
  //   printf ("%ld  \n off_name %x \n size_file %d \n off_cont %x \n  check_sum %lx \n ",   i,file_pointer[i]->off_name,file_pointer[i]->size_file, file_pointer[i]->off_cont,file_pointer[i]->check_sum);

  // }


  string f_str[n_files];
  for (size_t i = 0; i <n_files; i++){
    lseek(fd,pako->off_str + file_pointer[i]->off_name , SEEK_SET );

    char * c;

    do{
      read(fd,c,1);

      f_str[i] += (*c);


    }while((*c)!='\0');


      





  }
  

  for (size_t i = 0; i <n_files; i++){
  //  printf (" %x \n" ,pako->off_dat + file_pointer[i]->off_cont) ;
     
   lseek(fd,pako->off_dat + file_pointer[i]->off_cont , SEEK_SET );
    // for  (file_pointer[i]-> )
    uint32_t tmp = file_pointer[i]->size_file;
    uint64_t check_tmp=0;
    uint64_t* buf=new uint64_t;

    while(tmp>0){
      if (tmp > sizeof(uint64_t)){
        tmp -= sizeof(uint64_t) ;
      }
      else {
          tmp =0;
      }

      read(fd, buf ,sizeof(uint64_t));
      check_tmp=check_tmp ^ *buf;
      
    }
     
    if(file_pointer[i]-> check_sum ==check_tmp){

      lseek(fd,pako->off_dat + file_pointer[i]->off_cont , SEEK_SET );
      // printf (" %x \n" ,) ;
      //  cout<< f_str[i]<<endl;
      // string path  ="/tmp/inplab2test/"+f_str[i];
      string path = argv[2]+f_str[i];
      const char* c_path= path.c_str();
      int w_fd = creat(c_path, (S_IRWXU));
      //cout<<"w_fd: "<< w_fd<<endl;
      char c_tmp[file_pointer[i]->size_file];
      
      res=read(fd, c_tmp ,file_pointer[i]->size_file);
      // cout<<res<<endl;
      cout<<"w_fd: "<< w_fd<<endl;

      write(w_fd,  c_tmp, file_pointer[i]->size_file);
      // printf(" %s \n" ,c_tmp); 
      close(w_fd);
      
    }
  }



  close(fd);
  return 0;

}
