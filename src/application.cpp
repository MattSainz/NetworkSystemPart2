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
      a = new Network(1, pa, pb);
      b = new Network(1, pb, pa);
      break;
    case 2:
      a = new Network(0, pa, pb);
      b = new Network(1, pb, pa);
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
  string s = to_string((*((int*)(&param))));
  char* c_str = (char*) s.c_str();

  char large[101];
  memset(large,c_str[0],100);
  large[101] = '\0';
  cout << "msg: " << large << endl;

  int protocol_id = *((int*)(&param));
  for(int i = 0; i < 100; i++)
  {
    a->my_send(protocol_id, large);
      //network sleeps for app
  }

  pthread_exit(0);
}

void *Application::app_thread_b(void *param)
{
  string s = to_string((*((int*)(&param))));
  char* c_str = (char*) s.c_str();

  char large[101];
  memset(large,c_str[0],100);
  large[101] = '\0';

  int protocol_id = *((int*)(&param));
  for(int i = 0; i < 100; i++)
  {
    b->my_send(protocol_id, large);
      //network handles sleeping
  }

  pthread_exit(0);
}

void Application::gen_apps()
{
  pthread_t tmp1;
  pthread_t tmp2;
  for(int i = 1; i <= 4; i++)
  {
    cout << i+4 << endl;
    pthread_create(&tmp1, NULL, Application::app_thread_a, (void*) i+4);
    pthread_create(&tmp2, NULL, Application::app_thread_b, (void*) i+4);
  }

  pthread_join(tmp1, 0);
  pthread_join(tmp2, 0);
}
