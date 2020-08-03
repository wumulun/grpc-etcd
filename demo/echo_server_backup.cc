#include <iostream>
#include <memory>
#include <string>
#include<thread>
#include<unistd.h>
#include<map>

#include "echo_server.h"

int main(int argc, char** argv) {
  std::string new_server=CSeviceFind::Main2BackupCheck();
  std::string server_name=new_server,server_ip="0.0.0.0:50054";
  CSeviceFind::RegisterService(server_name,server_ip);
  CSeviceFind::Run();

  return 0;
}
