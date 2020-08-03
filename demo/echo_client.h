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

//grpc通信；
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

//etcd服务发现；
class CClientServiceFind
{
  public:
  CClientServiceFind(std::string str_ip):oClient(str_ip){
    Status status=oClient.getFromKey("server",etcd_map_pre);
    kv_iter=etcd_map_pre.begin();
    while(kv_iter!=etcd_map_pre.end()){
      std::cout<<kv_iter->first<<"  "<<kv_iter->second <<std::endl;
      ++kv_iter;
    }
    kv_iter=etcd_map_pre.begin();
  }

  void Run();

  private:
  Client oClient;
  std::map<std::string, std::string> etcd_map_cur,etcd_map_pre;
  std::map<std::string, std::string>::iterator kv_iter;

  std::string GetServer();
};

  

void CClientServiceFind::Run(){
  std::string msg="",server_ip="";
  std::cout<<"start!"<<std::endl;
  //范围查找示例；
while(std::getline(std::cin,msg)){
  server_ip=GetServer();
  EchoClient echo_client(grpc::CreateChannel(server_ip, grpc::InsecureChannelCredentials()));
  std::string reply = echo_client.SayEcho(msg);
  std::cout << reply << std::endl;
  }
}

std::string CClientServiceFind::GetServer(){
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
    ++kv_iter;
    return target_ip;
}



