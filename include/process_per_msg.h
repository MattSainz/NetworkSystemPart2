#ifndef PROCESS_PER_MSG
#define PROCESS_PER_MSG

#include <iostream>
#include <queue>
#include <semaphore.h>

#include "protocol.h"
#include "../network_libs/threadpool.h"


class ProcessPerMsg : public Protocol
{
  public:

    ProcessPerMsg();

    ~ProcessPerMsg();

    void net_send(Protocol::ProtoMsg* msg);

    void net_rec(Protocol::ProtoMsg* msg);

  private:

    void init_threads();

    const size_t POOL_SIZE = 25;

    static FP* send_fp;
    static FP* deliver_fp;

    //static sem_t send_queue_sem;
    //static pthread_mutex_t send_mu;
    //static std::queue<ProtoMsg*> to_send;

    static ThreadPool* pool;

    static void ethernet_deliver(void* msg);
    static void ip_deliver(void* msg);
    static void udp_deliver(void* msg);
    static void tcp_deliver(void* msg);
    static void ftp_deliver(void* msg);
    static void tel_deliver(void* msg);
    static void rdp_deliver(void* msg);
    static void dns_deliver(void* msg);

    static void ethernet_send(void* msg);
    static void ip_send(void* msg);
    static void udp_send(void* msg);
    static void tcp_send(void* msg);
    static void ftp_send(void* msg);
    static void tel_send(void* msg);
    static void rdp_send(void* msg);
    static void dns_send(void* msg);

    static void t_s(Message* m);

};
#endif