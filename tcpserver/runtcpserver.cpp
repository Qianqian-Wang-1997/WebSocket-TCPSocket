#include "tcpserver.h"
#include<iostream>
#include<csignal>
#include<ctime> //show time

#include <websocketpp/config/asio_no_tls.hpp>//websocketpp asio依赖
#include <websocketpp/server.hpp>           //websocket server依赖
//#include <websocketpp/http/request.hpp>     //http请求

typedef websocketpp::server<websocketpp::config::asio> server;  //websocket server
typedef server::message_ptr message_ptr;  //判断msg收到的数据类型
typedef websocketpp::http::parser::request request; //存储收到的http连接报头信息


//bind函数中过于长的函数名的alias
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

tcpserver tcp;
pthread_t msg_real[MAX_CLIENT_NO];
int num_message = 0;
int time_send = 2000;

//int i =  0; //debug
/********* websocket function ***********/


void on_open(websocketpp::connection_hdl hdl) {

}

//接收信息
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr wsmsg) {
  std::cout << "on_message with hdl: " << hdl.lock().get() << std::endl
            << "message: " << wsmsg->get_payload() << std::endl;

  websocketpp::frame::opcode::text;
  std::cout << "header: " << wsmsg->get_header() << "do i have?" << std::endl;
  //i++;
  //std::cout << "i:" << i << std::endl;
  s->send(hdl, wsmsg->get_payload(), wsmsg->get_opcode());
}

//void on_

/********* websocket function over***********/


/*********  tcpserver function ***********/
void* send_client(void* sock)
{
  struct socketDesc *desc = (struct socketDesc *)sock;

  while(1)
  {
    if(!tcp.isOnline() && tcp.GetLastClosed()==desc->id) //关闭某一client返回提示
    {
      cerr << "close client id:" << desc->id << endl<< "ip:" << desc->ip << endl;
      break;
    }
    //获取当前时间戳
    time_t t = time(0);
    tm *now = localtime(&t);
    string date = to_string(now->tm_hour) + ":" +
                  to_string(now->tm_min) + ":" +
                  to_string(now->tm_sec);
    cerr << date << endl;

    //发送给client回复ack
    tcp.Send(date, desc->id);
    cerr << "desc->id: "<< desc->id << endl;
    
    //超过时间提示链接仍在建立中
    sleep(time_send);
    cout << "SEND_CLIENT" << endl; 
  }
  pthread_exit(NULL);
  return 0;
}

void* receive_client(void * sock)
{

  pthread_detach(pthread_self()); //函数尾部自动退出线程
  vector<socketDesc *> desc; //当前收到的msg
  while(1)
  {
    // usleep(600); debuging
    // continue;

    desc = tcp.GetMessage();
    for (int i = 0; i < desc.size(); i++) {
      if(desc[i]->message !="") {

         if(desc[i]->count==false) {
           desc[i]->count = true; //建立socket完成 开始准备接收数据
           num_message++; //计数来的client个数
          }
         if(pthread_create(&msg_real[num_message], NULL, send_client, (void *) desc[i])==0) {
           cerr << "Thread create done" << endl;
        }
        cout << "id:       " << desc[i]->id << endl
             << "ip:       " << desc[i]->ip << endl
             << "message:  " << desc[i]->message << endl;
        tcp.Clean(i);
      }
    }
    usleep(600);
  }
  return 0;
}

/*程序退出时 关闭线程 */
void signal(int use)
{
  tcp.closed();
  exit(1);
}
/*********  tcpserver function over***********/


int main(int argc, char **argv)
{
  /* websocket */
  if (argc == 1)
  {
      server wbserver;

      wbserver.set_access_channels(websocketpp::log::alevel::all);
      wbserver.clear_access_channels(websocketpp::log::alevel::frame_payload);
      wbserver.init_asio();
      wbserver.set_message_handler(websocketpp::lib::bind(&on_message, &wbserver,::_1, ::_2)); //为message创建handler
      wbserver.listen(9002); //port
      wbserver.start_accept();//循环accept
      wbserver.run(); //循环接收client信息
  } 


/* tcpsocket */
  else if(argc == 2) {
      std::signal(SIGINT, signal);
      pthread_t msg;
      vector<int> opt = {SO_REUSEADDR, SO_REUSEPORT}; //若不设定 则默认opt为1
      if (tcp.setupSocket(6666, opt) == 0)
      {
        if(pthread_create(&msg, NULL, receive_client, (void*)0)==0)
        {
          while(1)
          {
            tcp.Accepted();
            cerr << "Accepted" << endl;
          }
        }
      }
      else
      {
        cerr << "socket setup error" << endl;
      }
      return 0; 
  }
}
