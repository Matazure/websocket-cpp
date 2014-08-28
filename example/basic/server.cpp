
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
        sp_socket->on_message([sp_socket](const std::string &s){
            std::cout << "on message: " << s << std::endl;
        });
        
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
