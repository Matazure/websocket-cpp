#include <websocket/websocket.hpp>


int main(){
    
    websocket::io_service iosev;
    
    //connect to server
   // auto sp_socket = websocket::connect(iosev, websocket::ip::tcp::endpoint(websocket::ip::tcp::v4(), 4231));
    {
        auto sp_socket = websocket::connect(iosev, "ws://manager:4231");
        
        sp_socket->send("4Hi Server");
        
        sp_socket->on_message([sp_socket](const std::string &s){
            std::cout << "on message: " << s << std::endl;
        });
        
        sp_socket->on_close([](){
            std::cout << "socket disconnect. " << std::endl;
        });
    }
    
    iosev.run();
    
    return 0;
}
