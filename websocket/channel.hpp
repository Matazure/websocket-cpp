#pragma once

#include <websocket/socket.hpp>

namespace websocket{
    
    class channel{
    public:
        typedef boost::signals2::signal<void (shared_ptr<socket>)>              connection_signal;
        typedef typename connection_signal::slot_type                           connection_slot;
        
        channel(): sp_connection_signal(new connection_signal){}
        
        void on_connection(connection_slot f){
            sp_connection_signal->connect(f);
        }
        
        
    public:
        shared_ptr<connection_signal>                                           sp_connection_signal;
    };
    
}
