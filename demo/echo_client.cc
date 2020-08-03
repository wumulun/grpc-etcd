#include"echo_client.h"


int main(int argc, char** argv) {
  CClientServiceFind client_impl("localhost:2379");
  client_impl.Run();
  return 0;
                   
}
