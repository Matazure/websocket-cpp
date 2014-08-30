
#include <websocket/websocket.hpp>

int main(){
    
    websocket::io_service iosev;
    //create an server
    
    auto sp_server = websocket::listen(iosev, 4231);
    
    sp_server->of("/chat")->on_connection([](std::shared_ptr<websocket::socket> sp_socket){
        sp_socket->send("I'm chat endpoint. ");
        
        sp_socket->on_message([](const std::string &s){
            std::cout << "chat channel message: " << s << std::endl;
        });
    });
    
    //connection event
    sp_server->on_connection([](std::shared_ptr<websocket::socket> sp_socket){
        //send message
        sp_socket->send("0{\"sid\":\"9SKCOKQByerbNyuiAAAA\",\"upgrades\":[],\"pingInterval\":25000,\"pingTimeout\":60000}");
        sp_socket->send("4Hi client.");
        
        //message event
        auto wp_socket = std::weak_ptr<websocket::socket>(sp_socket);
        sp_socket->on_message([wp_socket](const std::string &s){
            if (s == "2"){
                wp_socket.lock()->send("3");
            }
            
            std::cout << "on message: " << s << std::endl;
        });
        //error event
        sp_socket->on_error([](websocket::error_code ec){
            std::cout << "unexpected error" << std::endl;
           // std::cout << "error with code: " << ec << std::endl;
        });
        
        //send ping control frame
      //  sp_socket->ping("Hi");
        
        //disconnect event
        sp_socket->on_disconnect([](){
            std::cout << "disconnect. " << std::endl;
        });
    });

    //make sure to run this code. it's important
    iosev.run();

    return 0;
}
