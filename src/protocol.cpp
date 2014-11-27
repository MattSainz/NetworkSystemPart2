#include "../include/protocol.h"
#include "../include/network.h"

Protocol::FP *Protocol::up_fun;
Protocol::FP *Protocol::down_fun;

void Protocol::ethernet_up(ProtoMsg *to_process)
{
  to_process->other_info->msgStripHdr(4 + 8);
  to_process->hlp = (int) to_process->other_info->msgStripHdr(1)[0] - '0';
}

void Protocol::ip_up(ProtoMsg* to_process)
{
  to_process->other_info->msgStripHdr(3 + 12);
  to_process->hlp = (int) to_process->other_info->msgStripHdr(1)[0] - '0';
}

void Protocol::udp_up(ProtoMsg* to_process)
{
  to_process->other_info->msgStripHdr(3 + 4);
  to_process->hlp = (int) to_process->other_info->msgStripHdr(1)[0] - '0';
}

void Protocol::tcp_up(ProtoMsg* to_process)
{
  to_process->other_info->msgStripHdr(3 + 4);
  to_process->hlp = (int) to_process->other_info->msgStripHdr(1)[0] - '0';
}

void Protocol::ftp_up(ProtoMsg* to_process)
{
  to_process->hlp = 0;
  to_process->other_info->msgStripHdr(3 + 8);
}

void Protocol::tel_up(ProtoMsg* to_process)
{
  to_process->hlp = 0;
  to_process->other_info->msgStripHdr(3 + 8);
}

void Protocol::rdp_up(ProtoMsg* to_process)
{
  to_process->hlp = 0;
  to_process->other_info->msgStripHdr(3 + 12);
}

void Protocol::dns_up(ProtoMsg* to_process)
{
  to_process->hlp = 0;
  to_process->other_info->msgStripHdr(3 + 8);
}

void Protocol::ethernet_down(ProtoMsg *to_process) {
  Message* my_msg = to_process->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[8];
  memset(other_data, '0', 8);
  my_msg->msgAddHdr(other_data, 8);

  char* hlp = new char[1];
  hlp[0] = (char) to_process->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  Network::threadPush(to_process);

  to_process->hlp = 0;
}

void Protocol::ip_down(ProtoMsg* to_process) {
  Message* my_msg = to_process->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[12];
  memset(other_data, '0', 12);
  my_msg->msgAddHdr(other_data, 12);

  char* hlp = new char[1];
  hlp[0] = (char) to_process->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  to_process->hlp = 1;
}

void Protocol::udp_down(ProtoMsg* to_process) {
  Message* my_msg = to_process->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[4];
  memset(other_data, '0', 4);
  my_msg->msgAddHdr(other_data, 4);

  char* hlp = new char[1];
  hlp[0] = (char) to_process->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  to_process->hlp = 2;
}

void Protocol::tcp_down(ProtoMsg* to_process) {
  Message* my_msg = to_process->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[4];
  memset(other_data, '0', 4);
  my_msg->msgAddHdr(other_data, 4);

  char* hlp = new char[1];
  hlp[0] = (char) to_process->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);

  to_process->hlp = 2;
}

void Protocol::ftp_down(ProtoMsg* to_process) {

  Message* my_msg = to_process->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[8];
  memset(other_data, '0', 8);
  my_msg->msgAddHdr(other_data, 8);

  char* hlp = new char[1];
  hlp[0] = (char) to_process->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);

  to_process->hlp = 3;
}

void Protocol::tel_down(ProtoMsg* to_process) {
  Message* my_msg = to_process->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[8];
  memset(other_data, '0', 8);
  my_msg->msgAddHdr(other_data, 8);

  char* hlp = new char[1];
  hlp[0] = (char) to_process->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  to_process->hlp = 3;
}

void Protocol::rdp_down(ProtoMsg* to_process) {
  Message* my_msg = to_process->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[12];
  memset(other_data, '0', 12);
  my_msg->msgAddHdr(other_data, 12);

  char* hlp = new char[1];
  hlp[0] = (char) to_process->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);

  to_process->hlp = 4;
}

void Protocol::dns_down(ProtoMsg* to_process) {
  Message* my_msg = to_process->other_info;
  int msg_len = (int) my_msg->msgLen();
  my_msg->msgAddHdr(get_msg_len(msg_len), 3);

  char* other_data = new char[8];
  memset(other_data, '0', 8);
  my_msg->msgAddHdr(other_data, 8);

  char* hlp = new char[1];
  hlp[0] = (char) to_process->hlp + 48;
  my_msg->msgAddHdr(hlp, 1);
  to_process->hlp = 4;
}

char* Protocol::get_msg_len(int len)
{
  int i;
  int digits = 0;
  for( i = len; i > 0; i/=10) digits++;
  char* arr = new char[2];
  memset(arr, '0', 2);
  digits = 0;
  for( i = len; i > 0; i/=10) arr[2 - digits++] = (char) i%10 + 48;
  return arr;
}
