#include <iostream>
#include<stdio.h>
#include<fcntl.h>
typedef struct {
        uint32_t magic;
        int32_t  off_str;
        int32_t  off_dat;
        uint32_t n_files;
} pako_header_t;
typedef struct lab3
{
   
}FILE_E;
 

int main (int argc, char *argv[]){
  printf("We have %d arguments:\n", argc);
  for (int i = 0; i < argc; ++i) {
    printf("[%d] %s\n", i, argv[i]);
  }
  open(argv[1], O_RDONLY);
  read()
  return 0;
  
}
