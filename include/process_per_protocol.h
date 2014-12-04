#ifndef PROCESS_PER_PRO_H
#define PROCESS_PER_PRO_H

#include <unistd.h>
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
    void net_send(ProtoMsg* msg);

   /*
    * Take a message and send it up
    */
    void net_rec(ProtoMsg* msg);

    static int count[9];

  private:

    typedef struct{
      int read_fd;
      int write_fd;
    }my_fd;

    static pthread_mutex_t write_locks[17];

    static pthread_barrier_t threads_ready;

    static my_fd* send_fd[];

    static my_fd* receive_fd[];

    static void* pipe_send(void *arg);

    static void* pipe_recv(void *arg);

    void init_protocol_threads();

};

//helpers
char* msg_to_str(Message* msg);

#endif