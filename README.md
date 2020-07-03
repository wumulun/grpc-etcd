# gRPC-etcd服务发现

## 运行方式：
 mkdir build  
 cd build  
 cmake ..  
 make
 
然后启动对应的服务器和客户端。

## 文件说明：
1.src：etcd C++ client接口;  
2.demo：服务发现demo；  

## 功能说明：
1.通过gRPC实现echo服务器和客户端之间的通信，同时也是etcd和etcd C++ client之间通信的方式;  
2.多个服务器在etcd集群上注册IP信息供客户端获取并访问，做服务发现;  
3.并利用etcd的一致性存储，完成当有服务器变更时，服务器之间可以做主备切换，客户端也会将自己的访问均衡到新服务器上；  


## 展示：
登录服务器，则在集群中自动注册ip信息：  

![image](./resource/注册1)
![image](./resource/注册2)

客户端通过查询etcd，实现对不同服务器的轮询访问：  

![image](./resource/轮询)


当有新的服务器添加时，客户端更新服务器集群的信息，做简单的负载均衡：  

![image](./resource/负载均衡)

一个服务器挂掉，会有新的同key值服务器将其顶替，实现主备切换：  

![image](./resource/主备切换)



（etcd C++ API参考GitHub：https://github.com/maragkose/etcd3-client）
