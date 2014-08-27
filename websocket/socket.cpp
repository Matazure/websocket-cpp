#include <websocket/socket.hpp>

// if the asio lib execute the taskes by multiply thread, the http_parser is not safe.
#include <http_parser.h>
#include <websocket/detail/frame.hpp>
#include <websocket/detail/utilities.hpp>

std::pair<std::string, std::string>     header_tmp_pair;
std::map<std::string, std::string>      http_headers;

void clear_http_parser(){
    http_headers.clear();
}

extern "C"{
    //make sure thread safe, if use  <thread>
    int on_message_begin(http_parser *){
        
        return 0;
    }
    
    
    int on_url(http_parser *, const char *p, size_t len){
        
        return 0;
    }
    
    int on_status(http_parser *, const char *p, size_t len){
        
        return 0;
    }
    
    int on_header_field(http_parser *, const char *p, size_t len){
        header_tmp_pair.first = std::string(p, p+len);
        return 0;
    }
    
    int on_header_value(http_parser *, const char *p, size_t len){
        header_tmp_pair.second = std::string(p, p+len);
        http_headers.insert(header_tmp_pair);
        return 0;
    }
    
    int on_headers_complete(http_parser *){
        
        return 0;
    }
    
    int on_body(http_parser *, const char *p, size_t len){
        
        return 0;
    }
    
    int on_message_complete(http_parser *){
        
        return 0;
    }
    
}


namespace websocket{
    
    void socket::connect(ip::tcp::endpoint ep){
        auto self = shared_from_this();
        _asio_socket.async_connect(ep,[self](boost::system::error_code ec){
            if (ec){
                std::cout << "failed to connect" << std::endl;
                return;
            }
            
            self->handshake();
        });
    }
    
    void socket::send(const std::string &msg){
        auto sp_frame = make_shared<frame>(msg);
        send_frame(sp_frame);
    }
    
    void socket::send(std::string &&msg){
        auto sp_frame = make_shared<frame>(std::move(msg));
        send_frame(sp_frame);
    }
    
    void socket::ping(const std::string &app_data){
        assert(app_data.size() < 126);
        
        auto sp_frame = frame::create_ping_frame(app_data);
        send_frame(sp_frame);
    }
    
    void socket::pong(const std::string &app_data){
        auto sp_frame = frame::create_pong_frame(app_data);
        send_frame(sp_frame);
    }
    
    void socket::disconnect(){
        disconnect(1001, "");
    }
    
    void socket::disconnect(uint16_t code, const std::string &reason){
        assert(reason.size() < 124);
        
        auto self = shared_from_this();
        auto sp_frame = frame::create_close_frame(code, reason);
        auto f = [self, code](){
            self->_close_code = code;
            self->state(state_code::closing);
        };
        
        send_frame(sp_frame, f);
    }
    
    void socket::send_frame(shared_ptr<frame> sp_frame, std::function<void (void)> f){
        auto self = shared_from_this();
        _iosev.post([self, sp_frame, f](){
            bool process_free = self->_frames.empty();
            self->_frames.push(sp_frame);
            self->_write_callbacks[sp_frame] = f;
            if (process_free){
                self->do_write();
            }
        });
    }
    
    void socket::abnormally_close(error_code ec){
        if (is_closed()){
            return; //do none
        }else{
            error(ec);
            close_tcp_socket();
        }
    }
    
    void socket::close_tcp_socket(){
        _asio_socket.close();
        if (error() != error_code::null){
            (*_sp_error_signal)(error());
        }
        (*_sp_disconnect_signal)();
    }

    void socket::wait_handshake(shared_ptr<connection_signal> sp_connection_signal){
        auto self = shared_from_this();
        assert(_state != state_code::open);
        //receive request
        async_read_until(_asio_socket, _read_buf, "\r\n\r\n", [self, sp_connection_signal](boost::system::error_code ec, size_t len){
            //parser request
            std::string request;
            request.resize(len);
            self->_read_buf.sgetn(reinterpret_cast<char *>(&request[0]), len);
            
            http_parser request_parser;
            http_parser_init(&request_parser, HTTP_REQUEST);
            http_parser_settings settings;
            settings.on_message_begin = on_message_begin;
            settings.on_url = on_url;
            settings.on_status = on_status;
            settings.on_header_field = on_header_field;
            settings.on_header_value = on_header_value;
            settings.on_headers_complete = on_headers_complete;
            settings.on_body = on_body;
            settings.on_message_complete = on_message_complete;
            http_parser_execute(&request_parser, &settings, request.c_str(), request.size());
            
            //create response
            bool success = true;
            auto key = http_headers["Sec-WebSocket-Key"];
            std::string response;
            bool handshake = false;
            if (http_headers["Upgrade"] != "websocket" || key.empty()){
                response.append("HTTP/1.1 300  Bad Request\r\n");
                handshake = false;
            }else{
                auto accept_key = detail::generate_sec_websocket_accept(key);
                response.append("HTTP/1.1 101 Switching Protocols\r\n");
                response.append("Upgrade: websocket\r\n");
                response.append("Connection: Upgrade\r\n");
                response.append("Sec-WebSocket-Accept: ").append(accept_key).append("\r\n");
                // response.append("Sec-WebSocket-Protocol: chat\r\n");
                response.append("\r\n");
                handshake = true;
            }
            clear_http_parser();
            //send response
            async_write(self->_asio_socket, buffer(response), [self, sp_connection_signal, handshake](boost::system::error_code ec, size_t){
                if (ec){
                    return;  //do none
                }
                if (handshake){
                    self->state(state_code::open);
                    (*sp_connection_signal)(self);
                    self->do_read();
                    self->do_write();
                }else{
                    return; //do none
                }
            });
        });
    }

    void socket::handshake(){
        auto key = detail::generate_sec_websocket_key();
        std::string request;
        request.append("GET //engine.io/?EIO=2&transport=websocket HTTP/1.1\r\n");
        request.append("Upgrade: websocket\r\n");
        request.append("Host: localhost\r\n");
        request.append("Connection: Upgrade\r\n");
        request.append("Sec-WebSocket-Key: ").append(key).append("\r\n");
        request.append("Sec-WebSocket-Protocol: chat, superchat\r\n");
        request.append("Sec-WebSocket-Version: 8\r\n");
        request.append("\r\n");
        
        auto accept = detail::generate_sec_websocket_accept(key);
        auto self = shared_from_this();
        async_write(_asio_socket, buffer(request), [self, accept](boost::system::error_code ec, size_t){
            if (ec){
                assert(false);  ///\Todo
            }
            //receive response
            async_read_until(self->_asio_socket, self->_read_buf, "\r\n\r\n",  [self, accept](boost::system::error_code ec, size_t len){
                if (ec){
                    return;
                }
                
                std::string response;
                response.resize(len);
                self->_read_buf.sgetn(&response[0], len);
                
                http_parser request_parser;
                http_parser_init(&request_parser, HTTP_RESPONSE);
                http_parser_settings settings;
                settings.on_message_begin = on_message_begin;
                settings.on_url = on_url;
                settings.on_status = on_status;
                settings.on_header_field = on_header_field;
                settings.on_header_value = on_header_value;
                settings.on_headers_complete = on_headers_complete;
                settings.on_body = on_body;
                settings.on_message_complete = on_message_complete;
                http_parser_execute(&request_parser, &settings, response.c_str(), response.size());
             
                if (http_headers["Sec-WebSocket-Accept"] != accept){
                    std::cout << "error accept" << std::endl;
                }else{
                    self->state(state_code::open);
                    (*self->_sp_connect_signal)();
                    self->do_write();
                    self->do_read();
                }
            });
        });
    }

    void socket::do_read(){
        using detail::get_close_code;
        
        auto self = shared_from_this();
        auto needed_size = std::max(int64_t(0), int64_t(2 - self->_read_buf.size()));
        auto mut_buf = self->_read_buf.prepare(needed_size);
        self->_read_buf.commit(needed_size);
        async_read(_asio_socket, mut_buf, [self](boost::system::error_code ec, size_t len){
            if (ec){
                self->abnormally_close(error_code::transmition);
                return;
            }
            
            auto sp_frame = make_shared<detail::frame>();
            self->_read_buf.sgetn(reinterpret_cast<char *>(&(sp_frame->header)), 2);
            if(sp_frame->header.fin() == 0)         assert(false);
            if(sp_frame->header.rsv1() || sp_frame->header.rsv2() || sp_frame->header.rsv3()){
                self->abnormally_close(error_code::protocol);
                return;
            }
            
            auto receive_payload = [self, sp_frame](){
                auto needed_size = std::max(int64_t(0), int64_t(sp_frame->payload_length() - self->_read_buf.size()));
                auto mut_buf = self->_read_buf.prepare(needed_size);
                self->_read_buf.commit(needed_size);
                async_read(self->_asio_socket, mut_buf, [self, sp_frame](boost::system::error_code ec, size_t len){
                    if (ec){
                        self->abnormally_close(error_code::transmition);
                        return;
                    }
                    
                    sp_frame->resize_payload();
                    self->_read_buf.sgetn(reinterpret_cast<char *>(&(sp_frame->payload[0])), sp_frame->payload.size());
                    assert(self->_read_buf.size() == 0);
                    
                    if (sp_frame->header.masked()){
                        sp_frame->unmask();
                    }
                    
                    switch (sp_frame->header.opcode()){
                        case 0x00:
                            assert(false);
                        case 0x01: //text
                            (*self->_sp_message_signal)(sp_frame->payload);
                            break;
                        case 0x02: //binary
                            //no diffirence text, you can use std::string as binary if you like(//to do);
                            (*self->_sp_message_signal)(sp_frame->payload);
                            break;
                        case 0x08:
                            if (self->state() == state_code::closing){
                                if (get_close_code(sp_frame->payload) == self->_close_code){
                                    self->state(state_code::closed_clearnly);
                                    self->close_tcp_socket();
                                }else{
                                    self->state(state_code::closed_not_clearnly);
                                    self->error(error_code::closure);
                                    self->close_tcp_socket();
                                }
                            }else{
                                auto sp_close_frame = frame::create_close_frame(get_close_code(sp_frame->payload), "");
                                self->send_frame(sp_close_frame, [self](){
                                    self->state(state_code::closed_clearnly);
                                    self->close_tcp_socket();
                                });
                            }
                            return;
                        case 0x09: //Ping
                            self->pong(sp_frame->payload); //send Pong in response to Ping.
                            break;
                        case 0x0A: //Pong
                            break;
                        default:
                            assert(false);
                    }
                    
                    self->do_read();
                });
            };
            
            auto receive_mask_and_payload = [self, sp_frame, receive_payload](){
                if (sp_frame->header.masked()){
                    auto needed_size = std::max(int64_t(0), int64_t(4 - self->_read_buf.size()));
                    auto mut_buf = self->_read_buf.prepare(needed_size);
                    self->_read_buf.commit(needed_size);
                    async_read(self->_asio_socket, mut_buf, [self, sp_frame, receive_payload](boost::system::error_code ec, size_t len){
                        if (ec){
                            self->abnormally_close(error_code::transmition);
                            return;
                        }
                        
                        self->_read_buf.sgetn(reinterpret_cast<char *>(&(sp_frame->mask_key[0])), 4);
                        receive_payload();
                    });
                    
                }else{
                    receive_payload();
                }
            };
            
            if (sp_frame->header.payload_length() == 126){
                auto needed_size = std::max(int64_t(0), int64_t(2 - self->_read_buf.size()));
                auto mut_buf = self->_read_buf.prepare(needed_size);
                self->_read_buf.commit(needed_size);
                async_read(self->_asio_socket, mut_buf, [self, sp_frame, receive_mask_and_payload](boost::system::error_code ec, size_t len){
                    if (ec){
                        self->abnormally_close(error_code::transmition);
                        return;
                    }
                    
                    self->_read_buf.sgetn(reinterpret_cast<char *>(&(sp_frame->extended_payload_length16)), 2);
                    receive_mask_and_payload();
                });
            }else if(sp_frame->header.payload_length() == 127){
                auto needed_size = std::max(int64_t(0), int64_t(8 - self->_read_buf.size()));
                auto mut_buf = self->_read_buf.prepare(needed_size);
                self->_read_buf.commit(needed_size);
                async_read(self->_asio_socket, mut_buf, [self, sp_frame, receive_mask_and_payload](boost::system::error_code ec, size_t len){
                    if (ec){
                        self->abnormally_close(error_code::transmition);
                        return;
                    }
                    
                    self->_read_buf.sgetn(reinterpret_cast<char *>(&(sp_frame->extended_payload_length64)), 8);
                    receive_mask_and_payload();
                });
            }else{
                receive_mask_and_payload();
            }
        });
    }
    
    void socket::do_write(){
        if (_frames.empty() || !is_open()){
            return;
        }
        
        auto sp_frame = _frames.front();
        _frames.pop();
        
        if (is_client()){
            sp_frame->mask();
        }
        
        streambuf buf;
        std::ostream os(&buf);
        detail::write_frame(os, *sp_frame);
        auto self = shared_from_this();
        async_write(_asio_socket, buf, [self, sp_frame](boost::system::error_code ec, size_t len){
            if (ec || len == 0){
                self->abnormally_close(error_code::transmition);
                return;
            }
            
            if (self->_write_callbacks[sp_frame]){
                self->_write_callbacks[sp_frame]();
            }
            
            self->do_write();
        });
    }

    
}