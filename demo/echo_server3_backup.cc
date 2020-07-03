#include <iostream>
#include <memory>
#include <string>
#include<thread>
#include<unistd.h>
#include<map>

#include "echo_server.h"

int main(int argc, char** argv) {
  while(true){
    std::map<std::string, std::string> etcd_map;
    Status status=oClient.get("server3",etcd_map);
    if(etcd_map.empty()){
      break;
    }
  }
  std::cout<<"new server..."<<std::endl;
  int server_time_live_id=50054;
  std::string etcd_key="server3",etcd_value="0.0.0.0:50054";
  oClient.createLease(server_time_live_id,3);
  std::thread th_update(threadUpdateLease,server_time_live_id);
  std::thread th_run(RunServer,etcd_key,etcd_value,server_time_live_id);
  th_update.join();
  th_run.join();

  return 0;
}
