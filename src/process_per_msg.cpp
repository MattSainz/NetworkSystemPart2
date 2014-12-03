#include "../include/process_per_msg.h"

using namespace std;

//std::queue<Protocol::ProtoMsg*> ProcessPerMsg::to_send;
//sem_t ProcessPerMsg::send_mu;
//pthread_mutex_t ProcessPerMsg::to_send;

ThreadPool* ProcessPerMsg::pool;
Protocol::FP *ProcessPerMsg::deliver_fp;
Protocol::FP *ProcessPerMsg::send_fp;

ProcessPerMsg::ProcessPerMsg()
{
  deliver_fp= new FP[9];
  deliver_fp[0] = nullptr;
  deliver_fp[1] = &ethernet_deliver;
  deliver_fp[2] = &ip_deliver;
  deliver_fp[3] = &tcp_deliver;
  deliver_fp[4] = &udp_deliver;
  deliver_fp[5] = &ftp_deliver;
  deliver_fp[6] = &tel_deliver;
  deliver_fp[7] = &rdp_deliver;
  deliver_fp[8] = &dns_deliver;

  send_fp= new FP[9];
  send_fp[0] = nullptr;
  send_fp[1] = &ethernet_send;
  send_fp[2] = &ip_send;
  send_fp[3] = &tcp_send;
  send_fp[4] = &udp_send;
  send_fp[5] = &ftp_send;
  send_fp[6] = &tel_send;
  send_fp[7] = &rdp_send;
  send_fp[8] = &dns_send;

  pool = new ThreadPool(POOL_SIZE);
}

ProcessPerMsg::~ProcessPerMsg()
{
}

void ProcessPerMsg::net_send(Protocol::ProtoMsg* msg)
{
  if( pool->thread_avail() )
  {
    pool->dispatch_thread( send_fp[msg->hlp], (void*) msg);
  }
  else
  {
    cout << "Too many msg =( \n";
  }
}

void ProcessPerMsg::net_rec(Protocol::ProtoMsg* msg)
{
  if( pool->thread_avail() )
  {
    pool->dispatch_thread( &ethernet_deliver, (void*)msg);
  }
  else
  {
    cout << "Too many msg =( \n";
  }
}

void ProcessPerMsg::init_threads()
{
}

void ProcessPerMsg::ethernet_send(void* msg)
{
}

void ProcessPerMsg::ip_send(void* msg)
{
  ethernet_down(msg);
}

void ProcessPerMsg::udp_send(void* msg)
{
  ip_down(msg);
  ip_send(msg);
}

void ProcessPerMsg::tcp_send(void* msg)
{
  ip_down(msg);
  ip_send(msg);
}

void ProcessPerMsg::ftp_send(void* msg)
{
  tcp_down(msg);
  tcp_send(msg);
}

void ProcessPerMsg::tel_send(void* msg)
{
  tcp_down(msg);
  tcp_send(msg);
}

void ProcessPerMsg::rdp_send(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  udp_down(msg);
  udp_send(msg);
}

void ProcessPerMsg::dns_send(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  udp_down(msg);
  udp_send(msg);
}

void ProcessPerMsg::ethernet_deliver(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  (*up_fun[m->hlp])(msg);
  if(m->hlp > 0) (*deliver_fp[m->hlp])(msg);
}

void ProcessPerMsg::ip_deliver(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  (*up_fun[m->hlp])(msg);
  (*deliver_fp[m->hlp])(msg);
}

void ProcessPerMsg::tcp_deliver(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  (*up_fun[m->hlp])(msg);
  (*deliver_fp[m->hlp])(msg);
}

void ProcessPerMsg::udp_deliver(void* msg)
{
   Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  (*up_fun[m->hlp])(msg);
  (*deliver_fp[m->hlp])(msg);
}

void ProcessPerMsg::ftp_deliver(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  (*up_fun[m->hlp])(msg);
  cout << "Ftp recived: ";
  t_s(m->other_info);
  cout << endl;
}

void ProcessPerMsg::tel_deliver(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  (*up_fun[m->hlp])(msg);
  cout << "Tel recived: ";
  t_s(m->other_info);
  cout << endl;
}

void ProcessPerMsg::rdp_deliver(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  (*up_fun[m->hlp])(msg);
  cout << "rdp recived: ";
  t_s(m->other_info);
  cout << endl;
}

void ProcessPerMsg::dns_deliver(void* msg)
{
  Protocol::ProtoMsg* m = (Protocol::ProtoMsg*) msg;
  (*up_fun[m->hlp])(msg);
  cout << "dns recived: ";
  t_s(m->other_info);
  cout << endl;
}

void ProcessPerMsg::t_s(Message* m)
{
  char b[512];
  m->msgFlat(b);
  cout << b;
}
