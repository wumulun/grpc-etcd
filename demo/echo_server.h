#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "Client.hpp"
#include "EchoServer.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using EchoServer::YourEcho;
using EchoServer::EchoRequest;
using EchoServer::EchoReply;



// Logic and data behind the server's behavior.
class EchoServiceImpl final : public YourEcho::Service {
public:
  EchoServiceImpl(std::string str):server(str){}
private:
  std::string server;  
  Status SayEcho(ServerContext* context, const EchoRequest* request,
                  EchoReply* reply) override {
  std::string rep=server+" echo:"+request->message();
  std::cout<<rep<<std::endl;
  reply->set_message(rep);
  return Status::OK;
  }
};

//etcd服务发现;
class CSeviceFind
{
public:
  static void RegisterService(std::string server_name,std::string server_address);

  static void Run();
  
  static std::string Main2BackupCheck();

private:
  static Client oClient;
  static std::string etcd_key;
  static std::string etcd_value;
  static int lease_id;

  static void ThreadUpdateLease();

  static void RunServer();
};

Client CSeviceFind::oClient("localhost:2379");
std::string CSeviceFind::etcd_key;
std::string CSeviceFind::etcd_value;
int CSeviceFind::lease_id;

void CSeviceFind::RegisterService(std::string server_name,std::string server_address){
  std::cout<<"reigster sucess..."<<std::endl;
  etcd_key=server_name;
  etcd_value=server_address;
  lease_id=std::stoi(etcd_value.substr(8));
}

void CSeviceFind::Run(){
  oClient.createLease(lease_id,3);
  std::thread th_update(CSeviceFind::ThreadUpdateLease);
  std::thread th_run(CSeviceFind::RunServer);
  th_update.join();
  th_run.join();
}

void CSeviceFind::RunServer() {
  if(etcd_key.empty()||etcd_value.empty()){
    std::cout<<"the server name and server ip is null..."<<std::endl;
    return ;
  }
  
  EchoServiceImpl service(etcd_key);
  Status status;
  if(lease_id==0){
    status = oClient.put(etcd_key, etcd_value);
  }else{
    status = oClient.put(etcd_key, etcd_value,lease_id);
  }
  if( status.ok() == true ){
      std::cout<<"put successfully,the key:"<<etcd_key\
      <<", the value（server_address）:"<<etcd_value<<std::endl;
  }
  else{
    std::cout<<"the status is wrong,check the local ip..."<<std::endl;
    return ;
  }
  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(etcd_value, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  //std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

std::string CSeviceFind::Main2BackupCheck(){
  std::string msg="";
  std::map<std::string, std::string> etcd_map_cur,etcd_map_pre;
  Status status=oClient.getFromKey("server",etcd_map_pre);
  std::string new_server;
  std::cout<<"wait for instead of the useless ip..."<<std::endl;
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
  return new_server;
}

void CSeviceFind::ThreadUpdateLease(){
  while(true){
    oClient.updateLease(lease_id);
    sleep(1);
  }
}

