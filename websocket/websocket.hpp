#pragma once

#include <websocket/server.hpp>

namespace websocket{

    inline shared_ptr<socket> connect(io_service &iosev, ip::tcp::endpoint ep){
        auto sp_socket = make_shared<socket>(iosev);
        sp_socket->is_client(true);
        sp_socket->connect(ep);
        return sp_socket;
    }

    inline shared_ptr<server> listen(io_service &iosev, ip::tcp::endpoint ep){
        return make_shared<server>(iosev, ep);
    }

}
