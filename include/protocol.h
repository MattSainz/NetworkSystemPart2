#ifndef PROTOCOL
#define PROTOCOL

#include "../network_libs/message.h"


class Protocol
{

  public:
      /*
     * contains all the information a message needs while
     * it moves up and down the network stack
     */
    typedef struct
    {
      int hlp;
      Message* other_info;
    }ProtoMsg;

    typedef void (*FP)(Protocol::ProtoMsg*);

    static FP* up_fun;

    static FP* down_fun;

  protected:

    const int len_size= 3;
    static char* get_msg_len(int len);

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


    /**
     * Removes the correct header to a message and determines the next
     * higher level protocol
     * @return ProtoMsg contains the processed messaged
     */
     static void ethernet_up(ProtoMsg* to_process);
     static void ip_up(ProtoMsg* to_process);
     static void udp_up(ProtoMsg* to_process);
     static void tcp_up(ProtoMsg* to_process);
     static void ftp_up(ProtoMsg* to_process);
     static void tel_up(ProtoMsg* to_process);
     static void rdp_up(ProtoMsg* to_process);
     static void dns_up(ProtoMsg* to_process);

    /**
     * Adds the correct header from a message from a message and determines the
     * next protocol
     * @return ProtoMsg contains the processed messaged
     */
     static void ethernet_down(ProtoMsg* to_process);
     static void ip_down(ProtoMsg* to_process);
     static void udp_down(ProtoMsg* to_process);
     static void tcp_down(ProtoMsg* to_process);
     static void ftp_down(ProtoMsg* to_process);
     static void tel_down(ProtoMsg* to_process);
     static void rdp_down(ProtoMsg* to_process);
     static void dns_down(ProtoMsg* to_process);


};


#endif
