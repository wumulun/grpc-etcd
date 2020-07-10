#include <iostream>
#include <memory>
#include <string>
#include<thread>
#include<unistd.h>

#include "echo_server.h"

int main(int argc, char** argv) {
  if(argc<3){
    std::cout<<"the para is not 2...(server1  0.0.0.0:50051)"<<std::endl;
    return -1;
  }
  std::string etcd_key=argv[1],etcd_value=argv[2];
  int server_time_live_id=std::stoi(etcd_value.substr(8));
  oClient.createLease(server_time_live_id,3);
  std::thread th_update(threadUpdateLease,server_time_live_id);
  std::thread th_run(RunServer,etcd_key,etcd_value,server_time_live_id);
  th_update.join();
  th_run.join();
  return 0;
}
