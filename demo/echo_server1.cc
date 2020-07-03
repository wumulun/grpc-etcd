#include <iostream>
#include <memory>
#include <string>
#include<thread>
#include<unistd.h>

#include "echo_server.h"

int main(int argc, char** argv) {
  std::string etcd_key="server1",etcd_value="0.0.0.0:50051";
  RunServer(etcd_key,etcd_value);
  return 0;
}