#include <websocket/server.hpp>

namespace websocket{
    
    void server::do_accept(){
        auto sp_socket = std::make_shared<socket>(_iosev);
        auto self = shared_from_this();
        _sp_acceptor->async_accept(sp_socket->_asio_socket, [self, sp_socket](boost::system::error_code ec){
            if (!ec){
                sp_socket->wait_handshake(self->_sp_connection_signal);
            }
            self->do_accept();
        });
    }

}