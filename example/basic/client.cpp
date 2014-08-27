#include <websocket/websocket.hpp>


int main(){
    
    websocket::io_service iosev;
    auto sp_socket = websocket::connect(iosev, websocket::ip::tcp::endpoint(websocket::ip::tcp::v4(), 4231));
    
    sp_socket->send("4Hi Server");
    
    sp_socket->on_message([sp_socket](const std::string &s){
        std::cout << "on message: " << s << std::endl;
        
      //  sp_socket->disconnect();
    });
    
    sp_socket->on_disconnect([](){
        std::cout << "socket disconnect. " << std::endl;
    });
    
    iosev.run();
    return 0;
}
