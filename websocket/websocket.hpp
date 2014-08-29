#pragma once

#include <websocket/server.hpp>

namespace websocket{

//    inline shared_ptr<socket> connect(io_service &iosev, ip::tcp::endpoint ep){
//        auto sp_socket = make_shared<socket>(iosev);
//        sp_socket->is_client(true);
//        sp_socket->connect(ep);
//        return sp_socket;
//    }

    inline shared_ptr<server> listen(io_service &iosev, size_t port){
        auto sp_server = make_shared<server>(iosev);
        sp_server->listen(port);
        return sp_server;
    }

    inline shared_ptr<socket> connect(io_service &iosev, const std::string &url){
        auto sp_socket = make_shared<socket>(iosev);
        sp_socket->connect(url);
        return sp_socket;
    }

}
