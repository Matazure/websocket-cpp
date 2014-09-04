#pragma once

#include <map>
#include <queue>
#include <functional>
#include <iostream>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>

namespace websocket{

    using namespace boost::asio;
    using std::make_shared;
    using std::shared_ptr;
    using std::enable_shared_from_this;

    class server;
    
    namespace detail{
        class frame;
    }
    
    enum struct error_code{
        null = 0,
        closure = 1, //clode code not matched
        protocol = 2,
        transmition = 3
    };

    class socket: public enable_shared_from_this<socket>{
    public:
        enum struct state_code{
            connecting,
            open,
            closing,
            closed_clearnly,
            closed_not_clearnly
        };

    public:
        typedef detail::frame                                           frame;
        
    public:
        //server connection signal when opening handshake
        typedef boost::signals2::signal<void (shared_ptr<socket>)>      connection_signal;
        //socket connect signal
        typedef boost::signals2::signal<void ()>                        open_signal;
        typedef typename open_signal::slot_type                         open_slot;
        
        //when asio socket close, emit
        typedef boost::signals2::signal<void ()>                        close_signal;
        typedef typename close_signal::slot_type                        close_slot;
        
        typedef boost::signals2::signal<void (error_code)>               error_signal;
        typedef typename error_signal::slot_type                         error_slot;
        
        typedef boost::signals2::signal<void (const std::string &)>     message_signal;
        typedef typename message_signal::slot_type                      message_slot;
        
        typedef std::function<void (error_code ec)>                     send_callback_type;
        

    public:
        socket(io_service &iosev)
        : _iosev(iosev), _asio_socket(iosev)
        , _state(state_code::connecting)
        , _sp_open_signal(new open_signal)
        , _sp_close_signal(new close_signal)
        , _sp_error_signal(new error_signal)
        , _sp_message_signal(new message_signal)
        , _error_code(error_code::null)
        {}

        void connect(const std::string &url);

        //send text payload data
        void send(const std::string &msg);
        void send(std::string &&msg);
        
        void ping(const std::string &app_data);
        void pong(const std::string &app_data);
        
        //send disconnect control frame, then close.
        void disconnect();
        void disconnect(uint16_t code, const std::string &reason);

        //if is client, the payload data will be masked
        bool is_client() const                  { return _is_client; }
        void is_client(bool v)                  { _is_client = v; }
    
        bool is_open() const                    { return _state == state_code::open; }
        bool is_closed() const                  {
            return _state == state_code::closed_clearnly | _state == state_code::closed_not_clearnly;
        }
        
        std::string path() const{
            return _url_info.at("path");
        }
        
        state_code state() const                { return _state; }
        void state(state_code state)            { _state = state; }
        
        error_code error() const                { return _error_code; }
        void error(error_code ec)               { _error_code = ec; }
        
        //emit after success to handshake
        void on_open(open_slot f)         {  _sp_open_signal->connect(f); }
        //emit when socket close(even occur error)
        void on_close(close_slot f)   {  _sp_close_signal->connect(f); }
        //emit when occuring error
        void on_error(error_slot f)             {  _sp_error_signal->connect(f); }
        //emit when receive text payload data.
        void on_message(message_slot f)         {  _sp_message_signal->connect(f); }
        
        ~socket(){
            std::cout << "Release socket. " << std::endl;
        }
        
    private:
        void emit_connect(){
            (*_sp_open_signal)();
        }
        
        void emit_disconnect(){
            (*_sp_close_signal)();
        }
        
        void emit_error(error_code ec){
            (*_sp_error_signal)(ec);
        }
        
        void emit_message(const std::string &msg){
            (*_sp_message_signal)(msg);
        }

    private:
        
        void connect();
        
        void send_frame(shared_ptr<frame>, send_callback_type callback = send_callback_type());
        
        //emit error event(signal), then close
        void abnormally_close(error_code);
        
        //close asio socket,  emit  disconnect event(signal)
        void close_tcp_socket();

        void wait_handshake(shared_ptr<server> );

        void handshake();

        void do_read();

        void do_write();

        friend class server;

    private:
        void emit_open(){
            (*_sp_open_signal)();
        }
        
        void emit_close(){
            (*_sp_close_signal)();
        }
        
        
    private:
        class io_service                                            &_iosev;
        
        bool                                                        _is_client;
        state_code                                                  _state;
        
        //
        std::string                                                 _url;
        std::map<std::string, std::string>                          _url_info;
        std::map<std::string, std::string>                          _header;
        ip::tcp::resolver::iterator                                 _endpoint_iterator;
        
        ip::tcp::socket                                             _asio_socket;
        std::queue<shared_ptr<frame>>                               _frames;
        std::map<shared_ptr<frame>, send_callback_type>             _write_callbacks;
        streambuf                                                   _read_buf;
        
        shared_ptr<open_signal>                                     _sp_open_signal;
        shared_ptr<close_signal>                                    _sp_close_signal;
        shared_ptr<error_signal>                                    _sp_error_signal;
        shared_ptr<message_signal>                                  _sp_message_signal;
        
        uint16_t                                                    _close_code;
        error_code                                                  _error_code;
    };

}
