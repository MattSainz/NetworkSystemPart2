#include "../network_libs/message.h"

class Protocol
{

  public:
   /**
    * Each protocol class must define a way to send a
    * message over a network
    */
    virtual void net_send();

   /**
    * Each protocol class must define a way to receive a
    * message over a network
    */
    virtual void net_rec();

  private:

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
     * contains all the information a message needs while
     * it moves up and down the network stack
     */
    typedef struct{
      int hlp;
      Message* other_info;
      int data_len;
    }ProtoMsg;

    /**
     * Removes the correct header to a message and determines the next
     * higher level protocol
     * @return ProtoMsg contains the processed messaged
     */
    ProtoMsg* up(ProtoMsg* to_process);

    /**
     * Adds the correct header from a message from a message and determines the
     * next protocol
     * @return ProtoMsg contains the processed messaged
     */
    ProtoMsg* down(ProtoMsg* to_process);

};
