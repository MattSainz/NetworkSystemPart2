#include "../include/application.h"

using namespace std;

Network* Application::a;
Network* Application::b;

Application::Application(int mode)
{
  switch (mode)
  {
    case 0:
      a = new Network(0, 5006, 5007);
      b = new Network(0, 5007, 5006);
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      exit(-1);
  }
}

Application::~Application()
{
  delete a;
  delete b;
}

void *Application::app_thread_a(void *param)
{
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 20;


  int protocol_id = *((int*)(&param));
  const char* msg[2] = {"foo","bar"};
  for(int i = 0; i < 100; i++)
  {
    a->my_send(protocol_id,(char*) msg[i%2]);
  }

  nanosleep(&t, NULL);

  pthread_exit(0);
}

void *Application::app_thread_b(void *param)
{
  int protocol_id = *((int*)(&param));
  const char* msg[2] = {"bax","lolz"};
  for(int i = 0; i < 100; i++)
  {
    b->my_send(protocol_id, (char*) msg[i%2]);
  }

  pthread_exit(0);
}

void Application::gen_apps()
{
  pthread_t tmp;
  for(int i = 1; i <= 4; i++)
  {
    pthread_create(&tmp, NULL, Application::app_thread_a, (void*) i);
    pthread_create(&tmp, NULL, Application::app_thread_b, (void*) i+4);
  }

  sleep(1);//TODO just join on threads
}
