#ifndef PROCESS_PER_PRO_H
#define PROCESS_PER_PRO_H

#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include "protocol.h"

class ProcessPerProtocol : public Protocol
{

  public:

    ProcessPerProtocol();

    ~ProcessPerProtocol();

    /*
     * Take a message and send it down
     * returns lowest layer message
     */
    char* net_send(Message* msg);

   /*
    * Take a message and send it up
    */
    void net_rec(Message* msg);

  private:

    static int send_fd[];

    static int receive_fd[];

    static void* pipe_read(void* arg);

    static void* pipe_write(void* arg);

    void init_protocol_threads();

};

//helpers
char* msg_to_str(Message* msg);

#endif