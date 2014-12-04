#include "../include/application.h"

using namespace std;

Network* Application::a;
Network* Application::b;

Application::Application(int mode)
{
  int pa = (rand()%5000) + 1000;
  int pb = (rand()%5000) + 1000;
  cout << "Pa: " << pa << " pb: " << pb << endl;
  int pid = fork();
  if( pid == 0 )
  {
    switch (mode)
    {
      case 0:
        a = new Network(0, pa, pb);
        break;
      case 1:
        a = new Network(1, pa, pb);
        break;
      case 2:
        a = new Network(0, pa, pb);
        break;
      default:
        exit(-1);
    }
  }
  else
  {
   switch (mode)
   {
     case 0:
       a = new Network(0, pb, pa);
       break;
     case 1:
       a = new Network(1, pb, pa);
       break;
     case 2:
       a = new Network(1, pb, pa);
       break;
     default:
       exit(-1);
   }
 }

}

Application::~Application()
{
  cout << "Application is done\n";
  delete a;
}

void *Application::app_thread_a(void *param)
{
  struct timespec t;
  t.tv_sec = 0;

  string s = to_string((*((int*)(&param))));
  char* c_str = (char*) s.c_str();

  char large[101];
  memset(large,c_str[0],100);
  large[101] = '\0';

  int protocol_id = *((int*)(&param));
  for(int i = 0; i < 100; i++)
  {
    t.tv_nsec = 1000000;
    a->my_send(protocol_id, large);
      //network sleeps for app
    nanosleep(&t, NULL);
  }

  char c[1] = {'0'};
  a->my_send(0, c);

  pthread_exit(0);
};

void Application::gen_apps()
{
  std::string hlp_name[5] = {"Error","FTP","TelNet","RDP","DNS"};
  char buf[512];
  bzero(buf,512);
  Protocol::ProtoMsg* msg;

  pthread_t tmp1;
  for(int i = 1; i <= 4; i++)
  {
    pthread_create(&tmp1, NULL, Application::app_thread_a, (void*) i+4);
  }
  pthread_join(tmp1, 0);
  std::queue<Protocol::ProtoMsg*> received = a->my_receive();
  /*
  while( ! received.empty() )
  {
    msg = received.front();
    msg->other_info->msgFlat(buf);
    cout << hlp_name[msg->hlp-4] << " recived: " << buf << endl;
    received.pop();
  }
  */
}
