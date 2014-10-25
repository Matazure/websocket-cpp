#include <websocket/websocket.hpp>


int main(){
    
    websocket::io_service iosev;
    
  
    auto sp_socket = websocket::connect(iosev, "ws://localhost:4231?zhang&zhimin");
    
    sp_socket->send("4Hi Server");
    
    sp_socket->on_message([sp_socket](const std::string &s){
        std::cout << "on message: " << s << std::endl;
        if (s=="close"){
            sp_socket->disconnect();
        }
    });
    
    sp_socket->on_close([](){
        std::cout << "socket disconnect. " << std::endl;
    });

    
    iosev.run();
    
    return 0;
}
