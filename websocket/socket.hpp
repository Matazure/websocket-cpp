#pragma once

#include <map>
#include <queue>
#include <functional>
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
        typedef boost::signals2::signal<void ()>                        connect_signal;
        typedef typename connect_signal::slot_type                      connect_slot;
        
        //when asio socket close, emit
        typedef boost::signals2::signal<void ()>                        disconnect_signal;
        typedef typename disconnect_signal::slot_type                   disconnect_slot;
        
        typedef boost::signals2::signal<void (error_code)>               error_signal;
        typedef typename error_signal::slot_type                         error_slot;
        
        typedef boost::signals2::signal<void (const std::string &)>     message_signal;
        typedef typename message_signal::slot_type                      message_slot;

    public:
        socket(io_service &iosev)
        : _iosev(iosev), _asio_socket(iosev)
        , _state(state_code::connecting)
        , _sp_connect_signal(new connect_signal)
        , _sp_disconnect_signal(new disconnect_signal)
        , _sp_error_signal(new error_signal)
        , _sp_message_signal(new message_signal)
        , _error_code(error_code::null)
        {}

        void connect(ip::tcp::endpoint ep);

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
        
        state_code state() const                { return _state; }
        void state(state_code state)            { _state = state; }
        
        error_code error() const                { return _error_code; }
        void error(error_code ec)               { _error_code = ec; }
        
        //emit after success to handshake
        void on_connect(connect_slot f)         {  _sp_connect_signal->connect(f); }
        //emit when socket close(even occur error)
        void on_disconnect(disconnect_slot f)   {  _sp_disconnect_signal->connect(f); }
        //emit when occuring error
        void on_error(error_slot f)             {  _sp_error_signal->connect(f); }
        //emit when receive text payload data.
        void on_message(message_slot f)         {  _sp_message_signal->connect(f); }
        
        ~socket(){
            std::cout << "Release Socket. " << std::endl;
        }

    private:
        
        void send_frame(shared_ptr<frame>, std::function<void (void)> callback = std::function<void (void)>());
        
        //emit error event(signal), then close
        void abnormally_close(error_code);
        
        //close asio socket,  emit  disconnect event(signal)
        void close_tcp_socket();

        void wait_handshake(shared_ptr<connection_signal> );

        void handshake();

        void do_read();

        void do_write();

        friend class server;

    private:
        io_service                          &_iosev;
        
        bool                                                        _is_client;
        state_code                                                  _state;
        
        ip::tcp::socket                                             _asio_socket;
        std::queue<shared_ptr<frame>>                               _frames;
        std::map<shared_ptr<frame>, std::function<void (void)>>     _write_callbacks;
        streambuf                                                   _read_buf;
        
        shared_ptr<connect_signal>          _sp_connect_signal;
        shared_ptr<disconnect_signal>       _sp_disconnect_signal;
        shared_ptr<error_signal>            _sp_error_signal;
        shared_ptr<message_signal>          _sp_message_signal;
        
        uint16_t                                                    _close_code;
        
        error_code                                                  _error_code;
    };

}
