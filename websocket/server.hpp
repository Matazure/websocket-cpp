#pragma once

#include <websocket/socket.hpp>
#include <websocket/channel.hpp>

namespace websocket{

    class server: public enable_shared_from_this<server>{
    public:
        typedef boost::signals2::signal<void (shared_ptr<socket>)>          connection_signal;
        typedef typename connection_signal::slot_type                       connection_slot;
        typedef boost::signals2::signal<void (void)>                        listening_signal;
        typedef typename listening_signal::slot_type                        listening_slot;

        server(asio::io_service &iosev)
        : _iosev(iosev)
        ,_sp_acceptor(nullptr)
        , _sp_connection_signal(new connection_signal)
        , _sp_listening_signal(new listening_signal){

        }

        void listen(size_t port){
            _sp_acceptor = make_shared<asio::ip::tcp::acceptor>(_iosev, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
            this->do_accept();
            this->emit_listening();
        }

        void on_connection(connection_slot callback){
//            _sp_connection_signal->connect(callback);
            of("/")->on_connection(callback);
        }

        void on_listening(listening_slot f){
            _sp_listening_signal->connect(f);
        }

        shared_ptr<channel> of(const std::string &path){

            if (_sp_channels[path])
                return _sp_channels[path];
            else{
                _sp_channels[path] = make_shared<channel>();
                return _sp_channels[path];
            }
        }

    private:
        void do_accept();

        void emit_listening(){
            auto self = shared_from_this();
            _iosev.post([self](){
                (*(self->_sp_listening_signal))();
            });
        }

    private:
        asio::io_service                                  &_iosev;

        shared_ptr<asio::ip::tcp::acceptor>               _sp_acceptor;

        std::vector<shared_ptr<socket>>             _sockets;

        shared_ptr<connection_signal>               _sp_connection_signal;
        shared_ptr<listening_signal>                _sp_listening_signal;

        std::map<std::string, shared_ptr<channel>>  _sp_channels;
    };

}
