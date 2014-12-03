#include <iostream>
#include <unistd.h>
#include "../include/process_per_protocol.h"

using namespace std;

ProcessPerProtocol::my_fd* ProcessPerProtocol::send_fd[9];

ProcessPerProtocol::my_fd* ProcessPerProtocol::receive_fd[9];

pthread_mutex_t ProcessPerProtocol::write_locks[17];

int ProcessPerProtocol::count[9] = {0};

ProcessPerProtocol::ProcessPerProtocol()
{
  //id's for protocols start at 1
  for(int i = 1; i < 17; i++)
  {
    write_locks[i] = PTHREAD_MUTEX_INITIALIZER;
  }

  init_protocol_threads();
  sleep(1);
  //TODO add bar
}

ProcessPerProtocol::~ProcessPerProtocol()
{
  sleep(2);
  //TODO wait until done
}

void ProcessPerProtocol::net_send(Protocol::ProtoMsg* msg)
{
  int w_fd = send_fd[msg->hlp]->write_fd;
  pthread_mutex_lock(&write_locks[w_fd]);
  write(w_fd,(char*)msg, sizeof(ProtoMsg));
  pthread_mutex_unlock(&write_locks[w_fd]);
  delete msg;
}

void ProcessPerProtocol::net_rec(Protocol::ProtoMsg* msg)
{
  int w_fd = receive_fd[msg->hlp]->write_fd;
  pthread_mutex_lock(&write_locks[w_fd+8]);
  write(w_fd,(char*)msg,sizeof(ProtoMsg));
  pthread_mutex_unlock(&write_locks[w_fd+8]);
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
        pthread_mutex_lock(&write_locks[tmp->hlp]);
        write(send_fd[tmp->hlp]->write_fd,(char*)tmp, sizeof(ProtoMsg));
        pthread_mutex_unlock(&write_locks[tmp->hlp]);
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
        pthread_mutex_lock(&write_locks[tmp->hlp+8]);
        write(receive_fd[tmp->hlp]->write_fd,(char*)tmp, sizeof(ProtoMsg));
        pthread_mutex_unlock(&write_locks[tmp->hlp+8]);
      }
      else
      {
        tmp->other_info->msgFlat(data);
        cout << "Recived: " << data << endl;
      }

      delete tmp;

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
