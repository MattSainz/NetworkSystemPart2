#include <iostream>
#include "../include/process_per_protocol.h"

using namespace std;

int ProcessPerProtocol::send_fd[9];

int ProcessPerProtocol::receive_fd[9];

ProcessPerProtocol::ProcessPerProtocol()
{

}

ProcessPerProtocol::~ProcessPerProtocol()
{

}

char* ProcessPerProtocol::net_send(Message* msg)
{
  char* to_ret = "foo";

  return msg_to_str(msg);
}

void ProcessPerProtocol::net_rec(Message* msg)
{
  cout << "Net_rec: " << msg_to_str(msg) << endl;
}

//going down
void *ProcessPerProtocol::pipe_read(void *arg)
{
  return nullptr;
}

//going up
void *ProcessPerProtocol::pipe_write(void *arg)
{
  int fd = *((int*)(&arg));

  bool a = true;
  while(a)
  {
    //do transform
    write(fd, (void const *) fd, sizeof(fd));


  }
}

void ProcessPerProtocol::init_protocol_threads()
{
  pthread_t tmp;
  int fd[2];
  //starts at one to match protocol id's
  for( int i = 1; i < 9; i++)
  {
    pipe(fd);
    receive_fd[i] = fd[0];
    send_fd[i]    = fd[1];
    pthread_create(&tmp,NULL , ProcessPerProtocol::pipe_read ,(void*) fd[0]);
    pthread_create(&tmp,NULL , ProcessPerProtocol::pipe_write,(void*) fd[1]);
  }
}

char* msg_to_str(Message* msg)
{
  char* buf = new char[100];
  msg->msgFlat(buf);
  return buf;
}
