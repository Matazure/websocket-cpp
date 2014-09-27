
#include <websocket/websocket.hpp>

int main(){
    
    websocket::io_service iosev;
  
    auto sp_server = websocket::listen(iosev, 4232);
    
    //connection event
    sp_server->on_connection([](std::shared_ptr<websocket::socket> sp_socket){
        //send message
        std::vector<char> data(126, 's');
        sp_socket->send(std::string(data.begin(), data.end()));

        std::vector<char> data1(1024, 's');
        sp_socket->send(std::string(data1.begin(), data1.end()));
        
        std::vector<char> data2(std::numeric_limits<int16_t>::max(), 's');
        sp_socket->send(std::string(data2.begin(), data2.end()));
        
        std::vector<char> data7(std::numeric_limits<int16_t>::max()+1, 's');
        sp_socket->send(std::string(data7.begin(), data7.end()));

        
        std::vector<char> data3(std::numeric_limits<uint16_t>::max()*2, 's');
        sp_socket->send(std::string(data3.begin(), data3.end()));
        
        std::vector<char> data9(std::numeric_limits<uint16_t>::max()*2, 's');
        sp_socket->send(std::string(data9.begin(), data9.end()));
        
        std::vector<char> data8(65535*2, 's');
        sp_socket->send(std::string(data8.begin(), data8.end()));
        
        std::vector<char> data4(std::numeric_limits<uint16_t>::max()+12, 's');
        data4[0] = 'b';
        data4.back() = 'e';
        sp_socket->send(std::string(data4.begin(), data4.end()));
//
        sp_socket->ping("Hi");

        std::vector<char> data5(std::numeric_limits<uint16_t>::max()+12, 'h');
        sp_socket->send(std::string(data5.begin(), data5.end()));
        
        
        
        sp_socket->on_message([](const std::string &s){
            std::cout << "message size: " << s.size() << std::endl;
        });
        //error event
        sp_socket->on_error([](websocket::error_code ec){
            std::cout << "unexpected error" << std::endl;
            // std::cout << "error with code: " << ec << std::endl;
        });
        
        //disconnect event
        sp_socket->on_close([](){
            std::cout << "disconnect. " << std::endl;
        });
    });
    
    //make sure to run this code. it's important
    iosev.run();
    
    return 0;
}
