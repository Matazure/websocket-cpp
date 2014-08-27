#pragma once

#include <websocket/socket.hpp>

namespace websocket{

    class server: public enable_shared_from_this<server>{
    public:
        typedef boost::signals2::signal<void (shared_ptr<socket>)>          connection_signal;
        typedef typename connection_signal::slot_type                       connection_slot;

        server(io_service &iosev, ip::tcp::endpoint ep): _iosev(iosev), _endpoint(ep), _acceptor(iosev, _endpoint), _sp_connection_signal(new connection_signal){
            this->do_accept();
        }

        void on_connection(connection_slot callback){
            _sp_connection_signal->connect(callback);
        }

    private:
        void do_accept();
        
    private:
        io_service                                  &_iosev;

        ip::tcp::endpoint                           _endpoint;
        ip::tcp::acceptor                           _acceptor;

        std::vector<shared_ptr<socket>>             _sockets;
        
        shared_ptr<connection_signal>               _sp_connection_signal;
    };

}
