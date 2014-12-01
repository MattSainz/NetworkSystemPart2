#include <iostream>
#include <unistd.h>
#include "../include/process_per_protocol.h"

using namespace std;

ProcessPerProtocol::my_fd* ProcessPerProtocol::send_fd[9];

ProcessPerProtocol::my_fd* ProcessPerProtocol::receive_fd[9];

pthread_mutex_t ProcessPerProtocol::write_locks[8];

ProcessPerProtocol::ProcessPerProtocol()
{
  up_fun = new FP[9];
  up_fun[0] = nullptr;
  up_fun[1] = &ethernet_up;
  up_fun[2] = &ip_up;
  up_fun[3] = &tcp_up;
  up_fun[4] = &udp_up;
  up_fun[5] = &ftp_up;
  up_fun[6] = &tel_up;
  up_fun[7] = &rdp_up;
  up_fun[8] = &dns_up;

  down_fun    = new FP[9];
  down_fun[0] = nullptr;
  down_fun[1] = &ethernet_down;
  down_fun[2] = &ip_down;
  down_fun[3] = &tcp_down;
  down_fun[4] = &udp_down;
  down_fun[5] = &ftp_down;
  down_fun[6] = &tel_down;
  down_fun[7] = &rdp_down;
  down_fun[8] = &dns_down;

  init_protocol_threads();
  sleep(1);
  //TODO add bar
}

ProcessPerProtocol::~ProcessPerProtocol()
{

}

Protocol::ProtoMsg* ProcessPerProtocol::net_send(Protocol::ProtoMsg* msg)
{

  int w_fd = send_fd[msg->hlp]->write_fd;
  write(w_fd,(char*)msg, sizeof(ProtoMsg));
  return msg;
}

void ProcessPerProtocol::net_rec(Protocol::ProtoMsg* msg)
{
  int w_fd = receive_fd[msg->hlp]->write_fd;
  write(w_fd,(char*)msg,sizeof(ProtoMsg));
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
  char* data = new char[100];

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

      (*my_fun)(tmp);

      tmp->other_info->msgFlat(data);

      if( tmp->hlp != 0)
      {
        //TODO add mutex
        write(send_fd[tmp->hlp]->write_fd,(char*)tmp, sizeof(ProtoMsg));
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
  char* data = new char[100];

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

      (*my_fun)(tmp);

      if( tmp->hlp != 0)
      {
        //TODO add mutex
        write(receive_fd[tmp->hlp]->write_fd,(char*)tmp, sizeof(ProtoMsg));
      }
      else
      {
        tmp->other_info->msgFlat(data);
        cout << data << " from " << id << endl;
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
  char* buf = new char[100];
  msg->msgFlat(buf);
  return buf;
}
