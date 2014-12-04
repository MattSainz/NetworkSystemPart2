#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <semaphore.h>
#include <mutex>
#include <pthread.h>
#include <netinet/in.h>
#include <queue>
#include "../include/process_per_protocol.h"
#include "../include/process_per_msg.h"
/**
 * Simulates network for use in this programming assignment
 * Sends application traffic over udp connection and listens for
 * traffic over same udp connectiouuujkjn
 */
class Network
{
  public:
    /**
     * Establishes a UDP connection with another host
     * and creates a protocol object based on the mode
     * @param mode 0 for process per protocol 1 for process per message
     */
     Network(int mode, int out, int in);

    /**
     * tears down udp connection and cleans up
     */
    ~Network();

    /**
     * invokes the proper method for sending a message from an application level
     * to the other host
     * @param id specifies the id of the protocol to be used
     * @msg contains the message to be sent down the stack
     */
    void my_send(int id, char* msg);

   /**
    * Listens for incoming messages from other host and
    * once received begins sending the message up the network protocol stack
    * assumes no further calls to preform cleanup and timeing for this
    * assignment
    * @return char* the message to be read out by the application
    */
    std::queue<Protocol::ProtoMsg*> my_receive();

    static void threadPush(Protocol::ProtoMsg* new_msg);

    static void deliverMsg(Protocol::ProtoMsg* new_msg);

  private:

    typedef struct{
      int port;
      Protocol* my_protocol;
      int mode;
    }ThreadParam;

    typedef Protocol::ProtoMsg ProtoMsg;

    static sem_t send_queue_sem;
    static sem_t process_sem;

    static std::queue<ProtoMsg*> to_send;
    static std::queue<ProtoMsg*> to_p;

    static std::queue<ProtoMsg*> rec;

    static pthread_mutex_t queue_lock;

    static pthread_mutex_t send_lock;

    static pthread_mutex_t rcv_lock;

    static pthread_mutex_t live_lock;

    static pthread_mutex_t awk_lock;

    static bool live;

    static int done_awk;

    static pthread_barrier_t threads_ready;

    int out_port;

    int in_port;

    int my_mode;

    Protocol* my_proto;

    static void* udp_rcv(void* param);

    static void* udp_send(void* param);


    static ProtoMsg* threadPop();

    void spawn_threads();

    static void* process_send(void* arg);

};

#endif
