#include <websocket/websocket.hpp>


int main(){

    websocket::io_service iosev;


    auto sp_socket = websocket::connect(iosev, "ws://manager:4232?zhang&zhimin");

    // sp_socket->send("4Hi Server");

    sp_socket->on_message([sp_socket](const std::string &s){
        std::cout << "message size: " << s.size() << std::endl;
        std::cout << "content: " << s << std::endl;
    });

    sp_socket->on_close([](){
        std::cout << "socket disconnect. " << std::endl;
    });

    iosev.run();

    return 0;
}
