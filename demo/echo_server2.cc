#include <iostream>
#include <memory>
#include <string>
#include<thread>
#include<unistd.h>

#include "echo_server.h"

int main(int argc, char** argv) {
  /*std::string etcd_key="server2",etcd_value="0.0.0.0:50052";
  RunServer(etcd_key,etcd_value);
  return 0;*/
  int server_time_live_id=50052;
  std::string etcd_key="server2",etcd_value="0.0.0.0:50052";
  oClient.createLease(server_time_live_id,3);
  std::thread th_update(threadUpdateLease,server_time_live_id);
  std::thread th_run(RunServer,etcd_key,etcd_value,server_time_live_id);
  th_update.join();
  th_run.join();
  return 0;
}
