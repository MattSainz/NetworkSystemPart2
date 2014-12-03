#include <error.h>
#include "../include/network.h"

using namespace std;

std::queue<Protocol::ProtoMsg*> Network::to_send;
sem_t Network::send_queue_sem;
pthread_mutex_t Network::queue_lock;
pthread_mutex_t Network::send_lock;
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
      break;
    default:
      exit(-1);
  }

  queue_lock = PTHREAD_MUTEX_INITIALIZER;
  send_lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_barrier_init(&threads_ready, NULL, 3);
  sem_init(&send_queue_sem, 0, 0);

  spawn_threads();
}

Network::~Network()
{
  //sleep(1);
  //TODO fix
}

void Network::my_send(int id, char* msg)
{
  pthread_mutex_lock(&send_lock);
  ProtoMsg* new_p = new ProtoMsg;
  new_p->hlp = id;
  Message* new_msg = new Message(msg,strlen(msg)+1);
  new_p->other_info = new_msg;
  if(my_mode == 0)
  {
    ProcessPerProtocol* ppp = (ProcessPerProtocol*) my_proto;
    ppp->net_send(new_p);
  }
  else
  {
    ProcessPerMsg* ppm = (ProcessPerMsg*) my_proto;
    ppm->net_send(new_p);
  }
  pthread_mutex_unlock(&send_lock);
}

char* Network::my_receive()
{
  //TODO figure out if I need this or not
}

//TODO split up udp connection from other code
void* Network::udp_rcv(void* param)
{
  cout << "Ending\n";
  char buf[256];

  struct timespec t;
  t.tv_sec = 0;

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
    exit(1);
  }//attempt to use port 5006

  if(bind(sock, (struct sockaddr *) &my_address, sizeof(my_address)) < 0)
  {
    perror("Error on Binding");
    exit(1);
  }//attempt to bind sock

  ProtoMsg* tmp;
  pthread_barrier_wait(&threads_ready);
  //todo figure out how todo cleaner with virtual functions
  if(0 == local->mode)
  {
    ProcessPerProtocol* p = (ProcessPerProtocol*) local->my_protocol;
    while(1)
    {
      t.tv_nsec = rand()%100 + 100;
      bzero(buf,256);
      recvfrom(sock, buf, 256, 0, (struct sockaddr *)&other, &len);

      tmp = new ProtoMsg;
      tmp->hlp = buf[0] - 48;
      tmp->other_info = new Message(buf, strlen(buf)+1);

      p->net_rec( tmp );
      nanosleep(&t, NULL);
    }
  }
  else
  {
    ProcessPerMsg* p = (ProcessPerMsg*) local->my_protocol;
    while(1)
    {
      t.tv_nsec = rand()%100 + 100;
      bzero(buf,256);
      recvfrom(sock, buf, 256, 0, (struct sockaddr *)&other, &len);

      tmp = new ProtoMsg;
      tmp->hlp = buf[0] - 48;
      tmp->other_info = new Message(buf, strlen(buf)+1);

      p->net_rec( tmp );

      nanosleep(&t, NULL);
    }
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

  while (1)
  {

    char* buf = new char[512];

    threadPop()->other_info->msgFlat(buf);

    sendto(s, buf, strlen(buf), 0, (struct sockaddr *) &other, sizeof(other));

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

void Network::threadPush(ProtoMsg* new_msg)
{
  pthread_mutex_lock(&queue_lock);
  to_send.push(new_msg);
  pthread_mutex_unlock(&queue_lock);
  sem_post(&send_queue_sem);
}

Protocol::ProtoMsg* Network::threadPop()
{
  sem_wait(&send_queue_sem);
  pthread_mutex_lock(&queue_lock);
  ProtoMsg* to_ret = to_send.front();
  to_send.pop();
  pthread_mutex_unlock(&queue_lock);

  return to_ret;
}

