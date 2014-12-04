#include <error.h>
#include "../include/network.h"

using namespace std;

std::queue<Protocol::ProtoMsg*> Network::to_send;
std::queue<Protocol::ProtoMsg*> Network::to_p;
std::queue<Protocol::ProtoMsg*> Network::rec;
sem_t Network::send_queue_sem;
sem_t Network::process_sem;
pthread_mutex_t Network::queue_lock;
pthread_mutex_t Network::live_lock;
pthread_mutex_t Network::send_lock;
pthread_mutex_t Network::rcv_lock;
pthread_mutex_t Network::awk_lock;
pthread_barrier_t Network::threads_ready;
bool Network::live = true;
int Network::done_awk = 0;

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

  pthread_mutex_init(&queue_lock, NULL);
  pthread_mutex_init(&live_lock, NULL);
  pthread_mutex_init(&send_lock, NULL);
  pthread_mutex_init(&awk_lock, NULL);
  pthread_mutex_init(&rcv_lock, NULL);
  pthread_barrier_init(&threads_ready, NULL, 3);
  sem_init(&send_queue_sem, 0, 0);
  sem_init(&process_sem, 0, 0);

  spawn_threads();
}

Network::~Network()
{
  bool keep_running = true;
  while(keep_running)
  {
    pthread_mutex_lock(&send_lock);
    keep_running = (to_p.size() > 0);
    pthread_mutex_unlock(&send_lock);
  }


  pthread_mutex_lock(&live_lock);
  live = false;
  pthread_mutex_unlock(&live_lock);

  cout << "Network is done\n";
  pthread_mutex_destroy(&queue_lock);
  pthread_mutex_destroy(&send_lock);
  sem_close(&send_queue_sem);
  sem_close(&process_sem);
  sem_destroy(&send_queue_sem);
  sem_destroy(&process_sem);
}

void Network::my_send(int id, char* msg)
{
  ProtoMsg* new_p = new ProtoMsg;
  new_p->hlp = id;
  Message* new_msg = new Message(msg,strlen(msg)+1);
  new_p->other_info = new_msg;

  pthread_mutex_lock(&send_lock);
  to_p.push(new_p);
  pthread_mutex_unlock(&send_lock);
  sem_post(&process_sem);
}

std::queue<Protocol::ProtoMsg*> Network::my_receive()
{
  std::string hlp_name[5] = {"Error","FTP","TelNet","RDP","DNS"};
  char buf[512];
  bzero(buf,512);
  Protocol::ProtoMsg* msg;

  bool keep_running = true;
  while(keep_running)
  {
    pthread_mutex_lock(&send_lock);
    keep_running = (to_p.size() > 0);
    pthread_mutex_unlock(&send_lock);
  }

  keep_running = true;
  while(keep_running)
  {
    pthread_mutex_lock(&awk_lock);
    keep_running = (done_awk < 3);
    pthread_mutex_unlock(&awk_lock);
  }

  if( my_mode == 0)
  {
    delete (ProcessPerProtocol*) my_proto;
  }
  else
  {
    delete (ProcessPerMsg*) my_proto;
  }
  cout << "Got: " << rec.size() << " messages" << endl;
  while( ! rec.empty() )
  {
    msg = rec.front();
    msg->other_info->msgFlat(buf);
    //cout << hlp_name[msg->hlp-4] << " recived: " << buf << endl;
    rec.pop();
  }

  return rec;
}

//TODO split up udp connection from other code
void* Network::udp_rcv(void* param)
{
  char buf[256];

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
    while(live)
    {
      bzero(buf,256);
      recvfrom(sock, buf, 256, 0, (struct sockaddr *)&other, &len);

      tmp = new ProtoMsg;
      tmp->hlp = buf[0] - 48;

      if( tmp->hlp > 0)
      {
        tmp->other_info = new Message(buf, strlen(buf) + 1);
        p->net_rec(tmp);
      }
      else
      {

        pthread_mutex_lock(&awk_lock);
        done_awk++;
        cout << "Done Awk: " << done_awk << endl;
        pthread_mutex_unlock(&awk_lock);
      }
    }
  }
  else
  {
    ProcessPerMsg* p = (ProcessPerMsg*) local->my_protocol;
    while(live)
    {
      bzero(buf,256);
      recvfrom(sock, buf, 256, 0, (struct sockaddr *)&other, &len);

      tmp = new ProtoMsg;
      tmp->hlp = buf[0] - 48;
      if( tmp->hlp > 0)
      {
        tmp->other_info = new Message(buf, strlen(buf) + 1);
        p->net_rec(tmp);
      }
      else
      {
        pthread_mutex_lock(&awk_lock);
        done_awk++;
        pthread_mutex_unlock(&awk_lock);
      }
    }
  }
  close(sock);

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

  while (live)
  {

    char* buf = new char[512];

    threadPop()->other_info->msgFlat(buf);

    sendto(s, buf, strlen(buf), 0, (struct sockaddr *) &other, sizeof(other));

  }
  close(s);
}

void* Network::process_send(void* arg)
{
  cout << "Processing sends\n";
  ThreadParam* local = (ThreadParam*) arg;
  ProtoMsg* to_process;
  if(local->mode == 0)
  {
    ProcessPerProtocol* ppp = (ProcessPerProtocol*) local->my_protocol;
    while(live)
    {
      sem_wait(&process_sem);
      pthread_mutex_lock(&send_lock);
      to_process = to_p.front();
      to_p.pop();
      pthread_mutex_unlock(&send_lock);

      if( to_process->hlp > 0)
      {
        ppp->net_send(to_process);
      }
      else
      {
        threadPush(to_process);
      }

    }

  }
  else
  {
    ProcessPerMsg* ppm = (ProcessPerMsg*) local->my_protocol;
    while(live)
    {
      sem_wait(&process_sem);
      pthread_mutex_lock(&send_lock);
      to_process = to_p.front();
      to_p.pop();
      pthread_mutex_unlock(&send_lock);

      if( to_process->hlp > 0)
      {
        ppm->net_send(to_process);
      }
      else
      {
        threadPush(to_process);
      }
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
  pthread_create(&thread,NULL, Network::process_send, (void *) to_send);

  pthread_barrier_wait(&threads_ready);
  pthread_barrier_destroy(&threads_ready);
}

void Network::threadPush(ProtoMsg* new_msg)
{
  pthread_mutex_lock(&queue_lock);
  to_send.push(new_msg);
  sem_post(&send_queue_sem);
  pthread_mutex_unlock(&queue_lock);
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

void Network::deliverMsg(Protocol::ProtoMsg* new_msg)
{
  pthread_mutex_lock(&rcv_lock);
  rec.push(new_msg);
  pthread_mutex_unlock(&rcv_lock);
}

