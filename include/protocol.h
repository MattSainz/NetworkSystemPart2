#ifndef PROTOCOL
#define PROTOCOL

#include "../network_libs/message.h"


class Protocol
{

private:
  const int len_size= 3;
  protected:

    /*
     * defines the different sizes of headers for the
     * various network protocols
     * 0: error
     * 1: Ethernet
     * 2: IP
     * 3: TCP
     * 4: UDP
     * 5: ftp
     * 6: telnet
     * 7: RDP
     * 8: DNS
     */
    int net_protocols[9]= {-1,8,12,4,4,8,8,12,8};

    /*
     * nothing below ethernet
     * ip -> ethernet
     * udp/tcp -> ip
     * ftp/telnet -> tcp
     * RDP/DNS -> udp
     */
    int lower_mappings[9]= {-1,0,1,2,2,3,3,4,4};

    /*
     * contains all the information a message needs while
     * it moves up and down the network stack
     */
    typedef struct
    {
      int hlp;
      Message* other_info;
      int data_len;
    }ProtoMsg;

    /**
     * Removes the correct header to a message and determines the next
     * higher level protocol
     * @return ProtoMsg contains the processed messaged
     */
    int ethernet_up(Message* to_process);
    int ip_up(Message* to_process);
    int udp_up(Message* to_process);
    int tcp_up(Message* to_process);
    int ftp_up(Message* to_process);
    int tel_up(Message* to_process);
    int rdp_up(Message* to_process);
    int dns_up(Message* to_process);

    /**
     * Adds the correct header from a message from a message and determines the
     * next protocol
     * @return ProtoMsg contains the processed messaged
     */
    void ethernet_down(Message* to_process);
    void ip_down(Message* to_process);
    void udp_down(Message* to_process);
    void tcp_down(Message* to_process);
    void ftp_down(Message* to_process);
    void tel_down(Message* to_process);
    void rdp_down(Message* to_process);
    void dns_down(Message* to_process);
};

#endif
