websocket-cpp
==============
websocket-cpp is Websocket protocol implemented by C++ (need C++11 fetures). 

Why we do this
--------------
We need a C++ websocket has interfaces like ws(nodejs websocket). We also can use it in point to point communication instead of raw tcp socket. Many company has its C++ transmition protocol based on tcp socket, Why we not do it together based on websocket, do a better.

Feature
--------
The following websocket features is supported.
  1. websocket handshake.
  2. websocket send data frame(include text and binary).
  3. websocket send control frame.
  4. websocket closures.
  
all the features are agree with protocol (http://tools.ietf.org/html/rfc6455)

Install
--------
It's based on boost library(inlucde asio, signals, uuid, archive) and http-parser library(included by weboskcet). We build the websocket by gyp, you should know it.
  1. you should install boost and download http-parser.
  2. edit the websocket.gyp for the boost include and library path or set varialbe by gyp -D <varialbe>=<value>.
  3. build the project, and use the websocket include files and library file by youself
  4. If you also use gyp to build you application as example basic, you needn't do the step3.
  
How to use
----------
##server##
```c++

#include <websocket/websocket.hpp>

int main(){
    
    websocket::io_service iosev;
    //create an server
    auto sp_server = websocket::listen(iosev, websocket::ip::tcp::endpoint(websocket::ip::tcp::v4(), 4231));
    
    //connection event
    sp_server->on_connection([](std::shared_ptr<websocket::socket> sp_socket){
        //send message
        std::string ss("4hellow world");
        sp_socket->send(std::move(ss));
        sp_socket->send("I'm from server.");
        
        //message event
        //sp_socket->on_message([sp_socket](const std::string &s){
        //    std::cout << "on message: " << s << std::endl;
        //});
        //circular reference
        
        //error event
        sp_socket->on_error([](websocket::error_code ec){
            std::cout << "unexpected error" << std::endl;
           // std::cout << "error with code: " << ec << std::endl;
        });
        
        //send ping control frame
        sp_socket->ping("Hi");
        
        //disconnect event
        sp_socket->on_disconnect([](){
            std::cout << "disconnect. " << std::endl;
        });
    });

    //make sure to run this code. it's important
    iosev.run();

    return 0;
}
```
##client##
```c++
#include <websocket/websocket.hpp>


int main(){
    
    websocket::io_service iosev;
    
    //connect to server
    auto sp_socket = websocket::connect(iosev, websocket::ip::tcp::endpoint(websocket::ip::tcp::v4(), 4231));
    
    sp_socket->send("4Hi Server");
    
    sp_socket->on_message([sp_socket](const std::string &s){
        std::cout << "on message: " << s << std::endl;
    });
    
    sp_socket->on_disconnect([](){
        std::cout << "socket disconnect. " << std::endl;
    });
    
    iosev.run();
    
    return 0;
}
```
