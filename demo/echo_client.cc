#include"echo_client.h"


int main(int argc, char** argv) {
  Client oClient("localhost:2379");
  RunWithEtcd(oClient);
  return 0;
                   
}
