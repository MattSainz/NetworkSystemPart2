#ifndef PROTOCOL
#define PROTOCOL

#include "../network_libs/message.h"
#include <pthread.h>
#include <mutex>

class Protocol
{

  public:
    Protocol();
    ~Protocol();
      /*
     * contains all the information a message needs while
     * it moves up and down the network stack
     */
    typedef struct {
      int hlp;
      Message* other_info;
    }ProtoMsg;

    typedef void (*FP)(void*);

    static FP* up_fun;

    static FP* down_fun;

  protected:

    const int len_size= 3;
    static const char DATA_CHAR;

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
     static void ethernet_up(void *to_process);
     static void ip_up(void * to_process);
     static void udp_up(void* to_process);
     static void tcp_up(void* to_process);
     static void ftp_up(void* to_process);
     static void tel_up(void* to_process);
     static void rdp_up(void *to_process);
     static void dns_up(void* to_process);

    /**
     * Adds the correct header from a message from a message and determines the
     * next protocol
     * @return ProtoMsg contains the processed messaged
     */
     static void ethernet_down(void* to_process);
     static void ip_down(void* to_process);
     static void udp_down(void* to_process);
     static void tcp_down(void* to_process);
     static void ftp_down(void* to_process);
     static void tel_down(void* to_process);
     static void rdp_down(void* to_process);
     static void dns_down(void* to_process);

     static int processing;
     static pthread_mutex_t mu;
     static void am_processing();
     static void not_processing();

     static bool live;
     static pthread_mutex_t live_mu;

};


#endif
