#include <iostream>
#include <memory>
#include <string>
#include<thread>
#include<unistd.h>

#include "echo_server.h"

//输入server名和对应的ip；
int main(int argc, char** argv) {
  if(argc<3){
    std::cout<<"the para is not 2...(server1  0.0.0.0:50051)"<<std::endl;
    return -1;
  }

  std::string server_name=argv[1],server_ip=argv[2];
  CSeviceFind::RegisterService(server_name,server_ip);
  CSeviceFind::Run();
  return 0;
}
