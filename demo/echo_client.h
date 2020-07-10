#include <iostream>
#include <memory>
#include <string>
#include<map>
#include <grpcpp/grpcpp.h>
#include <thread>

#include "Client.hpp"
#include "EchoServer.grpc.pb.h"
#include "Watcher.hpp"



using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using EchoServer::YourEcho;
using EchoServer::EchoRequest;
using EchoServer::EchoReply;

class EchoClient {
 public:
  EchoClient(std::shared_ptr<Channel> channel)
      : stub_(YourEcho::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayEcho(const std::string& msg) {
    // Data we are sending to the server.
    EchoRequest request;
    request.set_message(msg);

    // Container for the data we expect from the server.
    EchoReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayEcho(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<YourEcho::Stub> stub_;
};

void RunWithEtcd(Client& oClient){
    std::string msg="";
    std::cout<<"start!"<<std::endl;
    std::map<std::string, std::string> etcd_map_cur,etcd_map_pre;
    Status status=oClient.getFromKey("server",etcd_map_pre);
    auto kv_iter=etcd_map_pre.begin();
    while(kv_iter!=etcd_map_pre.end()){
      std::cout<<kv_iter->first<<"  "<<kv_iter->second <<std::endl;
      ++kv_iter;
    }
    kv_iter=etcd_map_pre.begin();
   //范围查找示例；
  while(std::getline(std::cin,msg)){
    Status status=oClient.getFromKey("server",etcd_map_cur);
    if(status.ok()==true){
      if(etcd_map_cur!=etcd_map_pre){
        etcd_map_pre=etcd_map_cur;
        kv_iter=etcd_map_pre.begin();
        while(kv_iter!=etcd_map_pre.end()){
          std::cout<<kv_iter->first<<"  "<<kv_iter->second <<std::endl;
          ++kv_iter;
        }
      }
      
    }else{
      std::cout<<"the getFromKey is error...";
    }

    if(kv_iter==etcd_map_pre.end()){
      kv_iter=etcd_map_pre.begin();
    }
    std::string target_ip=kv_iter->second;
    EchoClient echo_client(grpc::CreateChannel(
     target_ip, grpc::InsecureChannelCredentials()));
    std::string reply = echo_client.SayEcho(msg);
    std::cout << reply << std::endl;
    ++kv_iter;
    
  }
}



