#include <iostream>
#include <memory>
#include <string>
#include<thread>
#include<unistd.h>

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

Client oClient("localhost:2379");

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

void threadUpdateLease(int lease_id){
  while(true){
    oClient.updateLease(lease_id);
    sleep(1);

  }
}

void RunServer(std::string etcd_key,std::string server_address,int lease_id=0) {
  EchoServiceImpl service(etcd_key);
  std::string etcd_value=server_address;
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
  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
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
