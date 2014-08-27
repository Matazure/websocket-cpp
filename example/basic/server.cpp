
#include <websocket/websocket.hpp>

int main(){
    
    websocket::io_service iosev;    
    auto sp_server = websocket::listen(iosev, websocket::ip::tcp::endpoint(websocket::ip::tcp::v4(), 4231));
    
    sp_server->on_connection([](std::shared_ptr<websocket::socket> sp_socket){
        std::string ss("4hellow world");
        sp_socket->send(std::move(ss));
        sp_socket->send("I'm from server.");
        
        sp_socket->on_message([sp_socket](const std::string &s){
            std::cout << "on message: " << s << std::endl;
   //         sp_socket->disconnect();
        });
        
        sp_socket->on_error([](websocket::error_code ec){
            std::cout << "unexpected error" << std::endl;
           // std::cout << "error with code: " << ec << std::endl;
        });
        
        sp_socket->ping("Hi");
        
        sp_socket->on_disconnect([](){
            std::cout << "disconnect. " << std::endl;
        });
    });

    iosev.run();

    return 0;
}
