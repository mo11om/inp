#!/bin/bash
# Program:
#       This program shows payload  in your screen.
# History:
# 2022/9/24 	First release
 
echo -e "Hello World! \a \n"
sudo tcpdump -c 8000 -v ip and src port 10001 -w test1.pcap 
python3 lab2.py test1.pcap
exit 0