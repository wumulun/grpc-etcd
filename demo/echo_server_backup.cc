#include <iostream>
#include <memory>
#include <string>
#include<thread>
#include<unistd.h>
#include<map>

#include "echo_server.h"

int main(int argc, char** argv) {
  Client oClient("localhost:2379");
    std::string msg="";
    
    std::map<std::string, std::string> etcd_map_cur,etcd_map_pre;
    Status status=oClient.getFromKey("server",etcd_map_pre);
    std::string new_server;
   //范围查找示例；
  while(true){
    Status status=oClient.getFromKey("server",etcd_map_cur);
    if(status.ok()==true){
      if(etcd_map_cur.size()<etcd_map_pre.size()){
        auto kv_iter=etcd_map_pre.begin();
        while(etcd_map_cur.find(kv_iter->first)!=etcd_map_cur.end()){
          std::cout<<kv_iter->first<<"  "<<kv_iter->second <<std::endl;
          ++kv_iter;
        }
        new_server=kv_iter->first;
        break;
      }
    }else{
        std::cout<<"the getFromKey is error...";
    }
    etcd_map_cur.clear();
    sleep(1);
  }
  
  std::cout<<"new server is insead of "<< new_server<<std::endl;
  int server_time_live_id=50054;
  std::string etcd_key=new_server,etcd_value="0.0.0.0:50054";
  oClient.createLease(server_time_live_id,3);
  std::thread th_update(threadUpdateLease,server_time_live_id);
  std::thread th_run(RunServer,etcd_key,etcd_value,server_time_live_id);
  th_update.join();
  th_run.join();

  return 0;
}
