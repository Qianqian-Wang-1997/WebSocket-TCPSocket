#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //提供对POSIX操作系统API的访问功能

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h> //htons htonl

#include <pthread.h> //多线程
#include <thread>
#include <algorithm>
#include <cctype>
#include <mutex> //保护信息不被多个线程共享 阻塞用 互斥锁

using namespace std;

#define MAXPACKETSIZE 128//40MB
#define MAX_CLIENT_NO 10 //最多50个客户端

struct socketDesc
{
  int socket = -1;
  string ip = "";
  int id = -1;
  string message;
  bool count = false;
};

class tcpserver
{
  public:
    static void* Task(void *arg); //处理线程
    int setupSocket(int port, vector<int> opts); //建立连接 setsocketopt,bind,listen
    void Accepted(); //接收客户端 多线程建立?
    vector<socketDesc *> GetMessage(); //返回已己收到的消息
    void Send(string msg, int id); //send()
    void Detach(int id);//关闭线程并初始化
    void Clean(int id); // message清空、
    bool isOnline(); //判断client是否在线 return isonline
    string GetIpAddr(int id); //return newsockfd[id]->ip
    int GetLastClosed(); //判断所有client已传递完毕
    void closed(); //关闭socket close(socketfd)
    void error(string errorMessage);

    int FetchHttpInfo(); //用于获取websocket握手包的信息
    

  private:
    int sockfd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    pthread_t ServThread[MAX_CLIENT_NO];

    //static
    static bool isonline; //判断该client是否在线
    static int last_closed; //判断所有client已传递完毕
    static int num_client;
    static std::mutex kMutex;

    static vector<socketDesc *> newSockfd; //保存client数据的addr，ip，id
    static char msg[MAXPACKETSIZE];
    //static vector<socketDesc *> Message;//保存cilent数据的msg
};

#endif
