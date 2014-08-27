#pragma once

#include <memory>
#include <array>
#include <vector>
#include <iostream>

namespace websocket{ namespace detail{

    using std::shared_ptr;
    using std::make_shared;

    struct frame{
        struct header{
            bool fin()  const                       { return 0x80&byte1; }
            bool rsv1() const                       { return 0x40&byte1; }
            bool rsv2() const                       { return 0x20&byte1; }
            bool rsv3() const                       { return 0x10&byte1; }
            uint8_t opcode() const                  { return 0x0F&byte1; }
            bool masked() const                     { return byte2&0x80; }
            uint8_t payload_length() const          { return byte2&0x7F; }

            void fin(bool v){
                if (v){
                    byte1 |= 0x80;
                }else{
                    byte1 &= 0x7F;
                }
            }

            void rsv1(bool v){
                if (v){
                    byte1 |= 0x40;
                }else{
                    byte1 &= 0xBF;
                }
            }

            void rsv2(bool v){
                if (v){
                    byte1 |= 0x20;
                }else{
                    byte1 &= 0xCF;
                }
            }

            void rsv3(bool v){
                if (v){
                    byte1 |= 0x10;
                }else{
                    byte1 &= 0xEF;
                }
            }

            void opcode(uint8_t v){
                assert(v < 0x0F);
                byte1   = (byte1&0xF0) | v;
            }

            void masked(bool v){
                if (v){
                    byte2 |= 0x80;
                }else{
                    byte2 &= 0x7F;
                }
            }

            void payload_length(uint8_t v){
                assert(v <= 0x7F);
                byte2 = (byte2&0x80) | v;
            }

            uint8_t    byte1;
            uint8_t    byte2;
        };

        void resize_payload(){
            payload.resize(payload_length());
        }

        void unmask(){
            //auto mask = header.mask_key;
            for (int i = 0; i < payload.size(); ++i){
                payload[i] ^= mask_key[i%4];
            }
        }
        
        void mask(){
            header.masked(true);
            srand(static_cast<unsigned>(time(nullptr)));
            auto ran = static_cast<int32_t>(rand());
            memcpy(&(mask_key[0]), &ran, 4);
            for (int i = 0; i < payload.size(); ++i){
                payload[i] ^= mask_key[i%4];
            }
        }
        
        frame(){
            ///do none
        }

        frame(const std::string &payload): payload(payload){
            init();
        }
        
        frame(std::string &&payload): payload(payload){
            init();
        }

    public:
        size_t payload_length(){
            uint64_t payload_length = header.payload_length();
            if (payload_length == 126){
                payload_length = extended_payload_length16;
            }else if (payload_length == 127){
                payload_length = extended_payload_length64;
            }

            return payload_length;
        }

        header                  header;
        int16_t                 extended_payload_length16;
        int64_t                 extended_payload_length64;
        std::array<uint8_t, 4>  mask_key;
        std::string             payload;
        
    public:
        static shared_ptr<frame> create_close_frame(uint16_t code, const std::string &reason){
            std::string payload;
            payload.resize(2);
            payload[0] = static_cast<uint8_t>(code >> 8);
            payload[1] = static_cast<uint8_t>(code);
            payload.append(reason);
            auto sp_frame = make_shared<frame>(std::move(payload));
            //0x08 close frame
            sp_frame->header.opcode(0x08);
            
            return sp_frame;
        }
        
        static shared_ptr<frame> create_ping_frame(const std::string &app_data){
            auto sp_frame = make_shared<frame>(app_data);
            //set opcode 0x09
            sp_frame->header.opcode(0x09);
            
            return sp_frame;
        }
        
        static shared_ptr<frame> create_pong_frame(const std::string &app_data){
            assert(app_data.size() < 126);
            
            auto sp_frame = make_shared<frame>(app_data);
            //set opcode 0x0A
            sp_frame->header.opcode(0x0A);
            
            return sp_frame;
        }
        
    private:
        void init(){
            header.fin(1);
            header.rsv1(0);
            header.rsv2(0);
            header.rsv3(0);
            header.opcode(0x01); //text data
            header.masked(0);
            if (payload.size() < 126){
                header.payload_length(payload.size());
            }else if(payload.size() < std::numeric_limits<uint16_t>::max()){
                header.payload_length(126);
                extended_payload_length16 = payload.size();
            }else{
                header.payload_length(127);
                extended_payload_length64 = payload.size();
            }
        }
    };

    //serialization
    void write_header(std::ostream &os,  const class frame::header &header){
        os.write(reinterpret_cast<const char *>(&(header.byte1)), 2);
    }

    void write_frame(std::ostream &os, const frame &frame){
        write_header(os, frame.header);

        if (frame.header.payload_length() == 126){
            os.write(reinterpret_cast<const char *>(&(frame.extended_payload_length16)), 2);
        }
        if (frame.header.payload_length() == 127){
            os.write(reinterpret_cast<const char *>(&(frame.extended_payload_length64)), 8);
        }
        if (frame.header.masked()){
            os.write(reinterpret_cast<const char *>(&(frame.mask_key[0])), 4);
        }

        os.write(reinterpret_cast<const char *>(&(frame.payload[0])), frame.payload.size());
    }

}}
