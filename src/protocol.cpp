#include "../include/protocol.h"
#include "../include/network.h"

Protocol::FP *Protocol::up_fun;
Protocol::FP *Protocol::down_fun;
pthread_mutex_t Protocol::mu;
pthread_mutex_t Protocol::live_mu;

int Protocol::processing = 0;
bool Protocol::live = true;
const char Protocol::DATA_CHAR = '-';

Protocol::Protocol()
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

  pthread_mutex_init(&mu, NULL);
  pthread_mutex_init(&live_mu, NULL);
}

Protocol::~Protocol()
{
  delete up_fun;
  delete down_fun;
  pthread_mutex_destroy(&mu);
  pthread_mutex_destroy(&live_mu);
}

void Protocol::ethernet_up(void *to_process)
{
  ProtoMsg* m = (ProtoMsg*) to_process;
  m->other_info->msgStripHdr(4 + 8);
  m->hlp = (int) m->other_info->msgStripHdr(1)[0] - '0';
}

void Protocol::ip_up(void* to_process)
{
  ProtoMsg* m = (ProtoMsg*) to_process;
  m->other_info->msgStripHdr(3 + 12);
  m->hlp = (int) m->other_info->msgStripHdr(1)[0] - '0';
}

void Protocol::udp_up(void* to_process)
{
  ProtoMsg* m = (ProtoMsg*) to_process;
  m->other_info->msgStripHdr(3 + 4);
  m->hlp = (int) m->other_info->msgStripHdr(1)[0] - '0';
}

void Protocol::tcp_up(void* to_process)
{
  ProtoMsg* m = (ProtoMsg*) to_process;
  m->other_info->msgStripHdr(3 + 4);
  m->hlp = (int) m->other_info->msgStripHdr(1)[0] - '0';
}

void Protocol::ftp_up(void* to_process)
{
  ProtoMsg* m = (ProtoMsg*) to_process;
  m->hlp = 0;
  m->other_info->msgStripHdr(3 + 8);
}

void Protocol::tel_up(void* to_process)
{
  ProtoMsg* m = (ProtoMsg*) to_process;
  m->hlp = 0;
  m->other_info->msgStripHdr(3 + 8);
}

void Protocol::rdp_up(void *to_process)
{
  ProtoMsg* m = (ProtoMsg*) to_process;
  m->hlp = 0;
  m->other_info->msgStripHdr(3 + 12);
}

void Protocol::dns_up(void* to_process)
{
  ProtoMsg* m = (ProtoMsg*) to_process;
  m->hlp = 0;
  m->other_info->msgStripHdr(3 + 8);
}

void Protocol::ethernet_down(void* to_process) {

  ProtoMsg* m = (ProtoMsg*) to_process;
  Message* my_msg = m->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[8];
  memset(other_data, DATA_CHAR, 8);
  my_msg->msgAddHdr(other_data, 8);

  char* hlp = new char[1];
  hlp[0] = (char) m->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);

  Network::threadPush(m);

  m->hlp = 0;

}

void Protocol::ip_down(void* to_process) {

  ProtoMsg* m = (ProtoMsg*) to_process;
  Message* my_msg = m->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[12];
  memset(other_data, DATA_CHAR, 12);
  my_msg->msgAddHdr(other_data, 12);

  char* hlp = new char[1];
  hlp[0] = (char) m->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  m->hlp = 1;

}

void Protocol::udp_down(void* to_process) {
  ProtoMsg* m = (ProtoMsg*) to_process;

  Message* my_msg = m->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[4];
  memset(other_data, DATA_CHAR, 4);
  my_msg->msgAddHdr(other_data, 4);

  char* hlp = new char[1];
  hlp[0] = (char) m->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  m->hlp = 2;

}

void Protocol::tcp_down(void* to_process) {
  ProtoMsg* m = (ProtoMsg*) to_process;

  Message* my_msg = m->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[4];
  memset(other_data, DATA_CHAR, 4);
  my_msg->msgAddHdr(other_data, 4);

  char* hlp = new char[1];
  hlp[0] = (char) m->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);

  m->hlp = 2;

}

void Protocol::ftp_down(void* to_process) {
  ProtoMsg* m = (ProtoMsg*) to_process;
  Message* my_msg = m->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[8];
  memset(other_data, DATA_CHAR, 8);
  my_msg->msgAddHdr(other_data, 8);

  char* hlp = new char[1];
  hlp[0] = (char) m->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);

  m->hlp = 3;

}

void Protocol::tel_down(void* to_process) {

  ProtoMsg* m = (ProtoMsg*) to_process;
  Message* my_msg = m->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[8];
  memset(other_data, DATA_CHAR, 8);
  my_msg->msgAddHdr(other_data, 8);

  char* hlp = new char[1];
  hlp[0] = (char) m->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  m->hlp = 3;

}

void Protocol::rdp_down(void* to_process) {
  ProtoMsg* m = (ProtoMsg*) to_process;
  Message* my_msg = m->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[12];
  memset(other_data, DATA_CHAR, 12);
  my_msg->msgAddHdr(other_data, 12);

  char* hlp = new char[1];
  hlp[0] = (char) m->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);

  m->hlp = 4;
}

void Protocol::dns_down(void* to_process) {
  ProtoMsg* m = (ProtoMsg*) to_process;
  Message* my_msg = m->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[8];
  memset(other_data, DATA_CHAR, 8);
  my_msg->msgAddHdr(other_data, 8);

  char* hlp = new char[1];
  hlp[0] = (char) m->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  m->hlp = 4;
}

char* Protocol::get_msg_len(int len)
{
  int i;
  int digits = 0;
  for( i = len; i > 0; i/=10) digits++;
  char* arr = new char[3];
  bzero(arr,3);
  memset(arr, '0', 2);
  digits = 0;
  for( i = len; i > 0; i/=10) arr[2 - digits++] = (char) i%10 + 48;
  return arr;
}

void Protocol::am_processing()
{
  pthread_mutex_lock(&mu);
  processing++;
  pthread_mutex_unlock(&mu);
}

void Protocol::not_processing()
{
  pthread_mutex_lock(&mu);
  processing--;
  pthread_mutex_unlock(&mu);
}
