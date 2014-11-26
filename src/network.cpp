#include "../include/network.h"

using namespace std;

std::queue<Message*> Network::to_send;
sem_t Network::send_queue_sem;
pthread_mutex_t Network::queue_lock;
pthread_barrier_t Network::threads_ready;

Network::Network(int mode, int out, int in)
{
  out_port = out;
  in_port  = in;
  my_mode = mode;

  switch (mode)
  {
    case 0:
      my_proto = new ProcessPerProtocol();
      break;
    case 1:
      my_proto = new ProcessPerMsg();
    default:
      exit(-1);
  }

  queue_lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_barrier_init(&threads_ready, NULL, 3);
  sem_init(&send_queue_sem, 0, 0);

  spawn_threads();
}

Network::~Network()
{
  while(!to_send.empty());
}

void Network::my_send(int id, char* msg)
{
  char* to_i = new char[2];
  sprintf(to_i, "%d", id);
  Message* new_msg = new Message(msg,strlen(msg));
  new_msg->msgAddHdr(to_i, 1);
  threadPush(new_msg);
}

char* Network::my_receive()
{
  //TODO figure out if I need this or not
}

//TODO split up udp connection from other code
void* Network::udp_rcv(void* param)
{
  char buf[1024];

  ThreadParam* local = (ThreadParam*) param;

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  struct sockaddr_in my_address, other;
  socklen_t len = sizeof(other);

  memset((char *) &my_address, 0, sizeof(my_address));
  my_address.sin_family = AF_INET;
  my_address.sin_addr.s_addr = INADDR_ANY;

  if((my_address.sin_port = htons(local->port)) == 0)
  {
    cout << "Error" << std::endl;
    exit(-1);
  }//attempt to use port 5006

  if(bind(sock, (struct sockaddr *) &my_address, sizeof(my_address)) < 0)
  {
    cout << "Error binding" << endl;
    exit(-1);
  }//attempt to bind sock

  pthread_barrier_wait(&threads_ready);
  if(0 == local->mode)
  {
    ProcessPerProtocol* ppp = (ProcessPerProtocol*) local->my_protocol;
    while(1)
    {
      recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&other, &len);
      ppp->net_rec( new Message(buf, strlen(buf)) );
    }
  }
  else
  {
    //TODO
  }

}

void* Network::udp_send(void* param)
{
  ThreadParam* local = (ThreadParam*) param;
  struct sockaddr_in other;
  int s;

  if( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    cout << "Error getting socket" << endl;
    exit(-1);
  }

  memset((char*) &other, 0, sizeof(other));
  other.sin_family        = AF_INET;
  other.sin_port          = htons(local->port);
  other.sin_addr.s_addr   = INADDR_ANY;

  pthread_barrier_wait(&threads_ready);
  if( local->mode == 0)
  {
    ProcessPerProtocol* ppp = (ProcessPerProtocol*) local->my_protocol;
    while (1)
    {
      char *to_send = ppp->net_send( threadPop() );

      cout << "Sending: " << to_send << endl;

      sendto(s, to_send, strlen(to_send), 0, (struct sockaddr *) &other, sizeof(other));

    }
  }

}

void Network::spawn_threads()
{
  ThreadParam* to_rcv = new ThreadParam;
  to_rcv->my_protocol = my_proto;
  to_rcv->port        = in_port;
  to_rcv->mode        = my_mode;

  ThreadParam* to_send= new ThreadParam;
  to_send->my_protocol = my_proto;
  to_send->port        = out_port;
  to_send->mode        = my_mode;

  pthread_t thread;
  pthread_create(&thread, NULL , Network::udp_send, (void *) to_rcv);
  pthread_create(&thread,NULL, Network::udp_rcv, (void *) to_send);

  pthread_barrier_wait(&threads_ready);
  pthread_barrier_destroy(&threads_ready);
}

void Network::threadPush(Message *new_msg)
{
  pthread_mutex_lock(&queue_lock);
  to_send.push(new_msg);
  pthread_mutex_unlock(&queue_lock);
  sem_post(&send_queue_sem);
}

Message *Network::threadPop()
{
  sem_wait(&send_queue_sem);
  pthread_mutex_lock(&queue_lock);
  Message* to_ret = to_send.front();
  to_send.pop();
  pthread_mutex_unlock(&queue_lock);

  return to_ret;
}
