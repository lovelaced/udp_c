# udp_c
UDP client and server in C with threaded echo reception

The blaster should be invoked in the following way:

__blaster -s \<hostname> -p \<port> -r \<rate> -n \<num> -q \<seq_no> -l \<length> -c \<echo>__

* hostname is the hostname of the blastee,
* port is the port on which the blastee is running,
* rate is the number of packets to be sent per second,
* num is the number of packets to be sent by the blaster,
* seq_no is the initial sequence of the packet exchange,
* length is the length of the variable payload in bytes in the packets, and
* echo parameter tells the blaster if it should expect the echo of its packet from the blastee. Values 1 (expect echo) and 0 (no echo) are valid for this parameter. 

The blastee receives the packets which the blaster has sent. It should be invoked in the following way:

__blastee -p \<port> -c \<echo>__

* port is the port on which the user wants to run blastee, and
* echo parameter tells the blastee if it should echo back the packet that it received to the blaster. Values 1 (echo back) and 0 (no echo) are valid for this parameter. 
