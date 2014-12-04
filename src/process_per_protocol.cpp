#include <iostream>
#include <unistd.h>
#include "../include/process_per_protocol.h"
#include "../include/network.h"

using namespace std;

ProcessPerProtocol::my_fd* ProcessPerProtocol::send_fd[9];

ProcessPerProtocol::my_fd* ProcessPerProtocol::receive_fd[9];

pthread_mutex_t ProcessPerProtocol::write_locks[17];

pthread_barrier_t ProcessPerProtocol::threads_ready;

int ProcessPerProtocol::count[9] = {0};

ProcessPerProtocol::ProcessPerProtocol()
{
  //id's for protocols start at 1
  for(int i = 1; i < 17; i++)
  {
    pthread_mutex_init( &(write_locks[i]), NULL);
  }
  pthread_barrier_init(&threads_ready, NULL, 17);
  init_protocol_threads();
  pthread_barrier_wait(&threads_ready);
  pthread_barrier_destroy(&threads_ready);
}

ProcessPerProtocol::~ProcessPerProtocol()
{
  bool keep_going = true;
  while(keep_going)
  {
    pthread_mutex_lock(&mu);
    keep_going = (processing > 0);
    pthread_mutex_unlock(&mu);
  }

  cout << "PPP is done " << endl;
}

void ProcessPerProtocol::net_send(Protocol::ProtoMsg* msg)
{
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 100;

  int w_fd = send_fd[msg->hlp]->write_fd;
  //pthread_mutex_lock(&write_locks[w_fd]);
  write(w_fd,(char*)msg, sizeof(ProtoMsg));
  //pthread_mutex_unlock(&write_locks[w_fd]);
  am_processing();
  delete msg;

  nanosleep(&t, NULL);
}

void ProcessPerProtocol::net_rec(Protocol::ProtoMsg* msg)
{
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 100;

  int w_fd = receive_fd[msg->hlp]->write_fd;
  //pthread_mutex_lock(&write_locks[w_fd+8]);
  write(w_fd,(char*)msg,sizeof(ProtoMsg));
  //pthread_mutex_unlock(&write_locks[w_fd+8]);
  am_processing();

  nanosleep(&t, NULL);
}

//going down
void *ProcessPerProtocol::pipe_send(void *arg)
{
  int id = *((int*)(&arg));
  my_fd* fds = send_fd[id];
  int read_fd = fds->read_fd;

  FP my_fun = down_fun[id];

  fd_set readset;
  int err = 0, size = 0;
  char* data = new char[512];

  ProtoMsg* tmp;

  pthread_barrier_wait(&threads_ready);
  while(1) {
    // Initialize the set
    FD_ZERO(&readset);
    FD_SET(read_fd, &readset);
    // Now, check for readability
    err = select(read_fd+1, &readset, NULL, NULL, NULL);
    if (err > 0 && FD_ISSET(read_fd, &readset))
    {
      tmp = new ProtoMsg;

      FD_CLR(read_fd, &readset);

      read(read_fd, (char*)tmp, sizeof(ProtoMsg));

      (*my_fun)((void*)tmp);

      if( tmp->hlp != 0)
      {
        //pthread_mutex_lock(&write_locks[tmp->hlp]);
        write(send_fd[tmp->hlp]->write_fd,(char*)tmp, sizeof(ProtoMsg));
        //pthread_mutex_unlock(&write_locks[tmp->hlp]);
      }
      else
      {
        not_processing();
      }

    }
  }
}

//going up
void *ProcessPerProtocol::pipe_recv(void *arg)
{

  int id = *((int*)(&arg));
  my_fd* fds = receive_fd[id];
  int read_fd = fds->read_fd;

  FP my_fun = up_fun[id];

  fd_set readset;
  int err = 0, size = 0;
  char* data = new char[512];

  ProtoMsg* tmp;

  pthread_barrier_wait(&threads_ready);
  while(1) {
    // Initialize the set
    FD_ZERO(&readset);
    FD_SET(read_fd, &readset);
    // Now, check for readability
    err = select(read_fd+1, &readset, NULL, NULL, NULL);
    if (err > 0 && FD_ISSET(read_fd, &readset))
    {
      tmp = new ProtoMsg;

      FD_CLR(read_fd, &readset);

      read(read_fd, (char*)tmp, sizeof(ProtoMsg));

      (*my_fun)((void*)tmp);

      if( tmp->hlp > 0)
      {
        //pthread_mutex_lock(&write_locks[tmp->hlp+8]);
        write(receive_fd[tmp->hlp]->write_fd,(char*)tmp, sizeof(ProtoMsg));
        //pthread_mutex_unlock(&write_locks[tmp->hlp+8]);
      }
      else if(tmp->hlp == 0)
      {
        tmp->hlp = id;
        Network::deliverMsg(tmp);
        not_processing();
      }

    }
  }
}

void ProcessPerProtocol::init_protocol_threads()
{
  pthread_t tmp;
  my_fd* fd_tmp;
  int fd[2];
  //starts at one to match protocol id's
  for( int i = 1; i < 9; i++)
  {
    pipe(fd);
    fd_tmp = new my_fd;

    fd_tmp->read_fd  = fd[0];
    fd_tmp->write_fd = fd[1];

    send_fd[i] = fd_tmp;

    pthread_create(&tmp, NULL, ProcessPerProtocol::pipe_send, (void *) i);

    pipe(fd);
    fd_tmp = new my_fd;

    fd_tmp->read_fd  = fd[0];
    fd_tmp->write_fd = fd[1];

    receive_fd[i] = fd_tmp;

    pthread_create(&tmp, NULL, ProcessPerProtocol::pipe_recv, (void *) i);
  }
}

char* msg_to_str(Message* msg)
{
  char* buf = new char[512];
  msg->msgFlat(buf);
  return buf;
}
