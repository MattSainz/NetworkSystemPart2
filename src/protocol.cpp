#include "../include/protocol.h"


int Protocol::ethernet_up(Message *to_process) {
  int to_ret = (int)to_process->msgStripHdr(1);
  to_process->msgStripHdr(3 + 8);
  return to_ret;
}

int Protocol::ip_up(Message *to_process) {
  int to_ret = (int)to_process->msgStripHdr(1);
  to_process->msgStripHdr(3 + 12);
  return to_ret;
}

int Protocol::udp_up(Message *to_process) {
  int to_ret = (int)to_process->msgStripHdr(1);
  to_process->msgStripHdr(3 + 4);
  return to_ret;
}

int Protocol::tcp_up(Message *to_process) {
  int to_ret = (int)to_process->msgStripHdr(1);
  to_process->msgStripHdr(3 + 4);
  return to_ret;
}

int Protocol::ftp_up(Message *to_process) {
  to_process->msgStripHdr(3 + 8);
  return 0;
}

int Protocol::tel_up(Message *to_process) {
  to_process->msgStripHdr(3 + 8);
  return 0;
}

int Protocol::rdp_up(Message *to_process) {
  to_process->msgStripHdr(3 + 8);
  return 0;
}

int Protocol::dns_up(Message *to_process) {
  to_process->msgStripHdr(3 + 8);
  return 0;
}

void Protocol::ethernet_down(Message *to_process) {

}

void Protocol::ip_down(Message *to_process) {
}

void Protocol::udp_down(Message *to_process) {
}

void Protocol::tcp_down(Message *to_process) {
}

void Protocol::ftp_down(Message *to_process) {
}

void Protocol::tel_down(Message *to_process) {
}

void Protocol::rdp_down(Message *to_process) {
}

void Protocol::dns_down(Message *to_process) {
}
