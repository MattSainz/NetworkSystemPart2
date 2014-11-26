#ifndef PROCESS_PER_MSG
#define PROCESS_PER_MSG

#include "protocol.h"

class ProcessPerMsg : public Protocol
{
  public:
    void net_send();

    void net_rec();

  private:


};
#endif