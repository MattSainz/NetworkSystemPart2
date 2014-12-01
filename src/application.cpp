#include "../include/application.h"

using namespace std;

Network* Application::a;
Network* Application::b;

Application::Application(int mode)
{
  int pa = (rand()%5000) + 1000;
  int pb = (rand()%5000) + 1000;
  switch (mode)
  {
    case 0:
      a = new Network(0, pa, pb);
      b = new Network(0, pb, pa);
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
    nanosleep(&t, NULL);
  }


  pthread_exit(0);
}

void *Application::app_thread_b(void *param)
{
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 20;

  int protocol_id = *((int*)(&param));
  const char* msg[2] = {"bax","lolz"};
  for(int i = 0; i < 100; i++)
  {
    b->my_send(protocol_id, (char*) msg[i%2]);
    nanosleep(&t, NULL);
  }

  pthread_exit(0);
}

void Application::gen_apps()
{
  pthread_t tmp;
  for(int i = 1; i <= 4; i++)
  {
    cout << i+4 << endl;
    pthread_create(&tmp, NULL, Application::app_thread_a, (void*) i+4);
    pthread_create(&tmp, NULL, Application::app_thread_b, (void*) i+4);
  }

  sleep(1);//TODO just join on threads
}
