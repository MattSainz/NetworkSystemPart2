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
    ProtoMsg* net_send(ProtoMsg* msg);

   /*
    * Take a message and send it up
    */
    void net_rec(ProtoMsg* msg);

  private:

    typedef struct{
      int read_fd;
      int write_fd;
    }my_fd;

    static my_fd* send_fd[];

    static my_fd* receive_fd[];

    static void* pipe_send(void *arg);

    static void* pipe_recv(void *arg);

    void init_protocol_threads();

};

//helpers
char* msg_to_str(Message* msg);

#endif