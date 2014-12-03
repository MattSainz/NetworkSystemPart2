#include <iostream>
#include "../include/network.h"
#include "../include/application.h"

using namespace std;

int main()
{
  cout << "Starting Network " << endl;
  Application* app_0 = new Application(1);
  app_0->gen_apps();
  delete app_0;
  cout << "Ending Network " << endl;
  return 0;
}

