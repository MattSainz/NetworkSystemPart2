#include "../include/network.h"
#include <iostream>
#include <pthread.h>
#include <time.h>

class Application
{
  public:
    Application(int mode);

    ~Application();

    void gen_apps();

  private:
    static void* app_thread_a(void* param);

    static void* app_thread_b(void* param);

    static Network* a;

    static Network* b;

};