
import sys
import base64
from scapy.all import *



def parse_pcap(pcap_path ):
    pcap_flow = rdpcap(pcap_path)
    for packet in pcap_flow:
        if packet.ttl >128:
            print (packet.ttl)
            print (packet.tos)
            print (base64.b64decode (bytes(packet[TCP].payload)))
     

def main(arguments):
    parse_pcap(arguments[1] )


if __name__ == "__main__":
    main(sys.argv)
