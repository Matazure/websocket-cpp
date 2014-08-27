#pragma once

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/uuid/sha1.hpp>

namespace websocket{namespace detail{

    inline std::string generate_sec_websocket_key(){
        srand (time(NULL));
        int16_t ran = rand() % 10 + 1;
        char *p= reinterpret_cast<char *>(&ran);

        typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<char *, 6, 8>> base64_iterator;
        base64_iterator base64_begin(p);
        base64_iterator base64_end(p+16);
        std::stringstream ss;
        copy(base64_begin, base64_end, std::ostream_iterator<char>(ss));
        ss << "==";
        return ss.str();
    }

    inline std::string generate_sec_websocket_accept(const std::string &key){
        //append 258EAFA5-E914-47DA-95CA-C5AB0DC85B11
        auto tmp1 = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        boost::uuids::detail::sha1 sha1;
        sha1.process_bytes(tmp1.c_str(), tmp1.size());
        uint32_t sha1_result[5];
        sha1.get_digest(sha1_result);

        char *sha1_bytes = new char[20];
        for (size_t i = 0; i < 5; ++i){
            auto p_start = reinterpret_cast<char *>(sha1_result);
            sha1_bytes[i*4] = p_start[i*4 + 3];
            sha1_bytes[i*4+1] = p_start[i*4+2];
            sha1_bytes[i*4+2] = p_start[i*4+1];
            sha1_bytes[i*4+3] = p_start[i*4];
        }

        //base64 encode
        typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<char *, 6, 8>> base64_iterator;
        base64_iterator base64_begin(sha1_bytes);
        base64_iterator base64_end(sha1_bytes+20);
        std::stringstream ss;
        copy(base64_begin, base64_end, std::ostream_iterator<char>(ss));
        ss << "=";
        delete[] sha1_bytes;
        assert(ss.str().size() == 28);
        return ss.str();
    }
    
    inline uint16_t get_close_code(const std::string &data){
        if (data.size() < 2){
            return uint16_t(1005);
        }else{
            uint16_t code;
            code = uint8_t(data[0]) << 8;
            code += uint8_t(data[1]);
            
            return code;
        }
    }

}}
