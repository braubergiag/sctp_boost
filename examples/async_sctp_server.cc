#include <cstdio>
#include <iostream>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <cstdio>
#include <array>
#include <functional>
#include <iostream>

namespace seqpacket {
    struct seqpacket_protocol {

 


        int type() const {
            return SOCK_SEQPACKET;
        }

        int protocol() const {
            return IPPROTO_SCTP;
        }

        int family() const {
            return AF_INET;
        }

        using endpoint = boost::asio::generic::seq_packet_protocol::endpoint;
        using socket = boost::asio::generic::seq_packet_protocol::socket;
        using acceptor = boost::asio::basic_socket_acceptor<seqpacket_protocol>;

        #if !defined(BOOST_ASIO_NO_IOSTREAM)
            using iostream = boost::asio::basic_socket_iostream<seqpacket_protocol>;
        #endif

    };
}


using seqpacket::seqpacket_protocol;

class session : public boost::enable_shared_from_this<session> {
    private:
        seqpacket_protocol::socket socket_;
        boost::array<char,1024> data_;

        seqpacket_protocol::socket::message_flags in_flags = MSG_WAITALL, out_flags = MSG_WAITALL;

    public:
        session(boost::asio::io_service & io_service) : socket_(io_service) {};
        seqpacket_protocol::socket & socket() {return socket_;}
        void start(){
            socket_.async_receive(boost::asio::buffer(data_),in_flags,
                boost::bind(&session::handle_read,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        }
        void handle_read(const boost::system::error_code & error,
                        size_t bytes_transferred){

            if (!error){
                socket_.async_send(boost::asio::buffer(data_,bytes_transferred),
                out_flags,boost::bind(&session::handle_write,
                            shared_from_this(),
                            boost::asio::placeholders::error));
            }else {
                std::cerr << "handle_read\n";
            }

        }
        void handle_write(const boost::system::error_code& error){
            if (!error){
                socket_.async_receive(boost::asio::buffer(data_),in_flags,
                boost::bind(&session::handle_read,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
            } else {
                std::cerr << "handle_write\n";
            }
        }

};

typedef boost::shared_ptr<session> session_ptr;
class server {
    private:
        boost::asio::io_service& io_service_;
        boost::asio::basic_socket_acceptor<boost::asio::generic::seq_packet_protocol> acceptor_;
    public:
        server(boost::asio::io_service& io_service,unsigned short port) 
             :  io_service_(io_service),
                acceptor_(io_service){
                    
                
                struct sockaddr_in servaddr;
                servaddr.sin_family = AF_INET;
                servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
                servaddr.sin_port = htons(port);
                

                boost::asio::generic::seq_packet_protocol p(AF_INET,IPPROTO_SCTP);
                seqpacket_protocol::endpoint endpoint((struct sockaddr *)&servaddr, sizeof(servaddr),IPPROTO_SCTP);
                acceptor_.open(p);
                acceptor_.bind(endpoint);

                 session_ptr new_session(new session(io_service_));
                    acceptor_.async_accept(new_session->socket(),
                    boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));

             }
               void handle_accept(session_ptr new_session,
                const boost::system::error_code& error){
                    if (!error)
                    {
                    new_session->start();
                    }else {
                    std::cerr << error.message()  << "\n";
                    exit(1);
                 }

                new_session.reset(new session(io_service_));
                acceptor_.async_accept(new_session->socket(),
                    boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
            }


            ~server(){


            }


    
};

int main(int argc, char * argv[]){


     try
        {
            if (argc != 2)
            {
            std::cerr << "Usage: stream_server <port>\n";
            std::cerr << "*** WARNING: existing file is removed ***\n";
            return 1;
            }

            boost::asio::io_service io_service;

            unsigned short port = atoi(argv[1]);

            server s(io_service, port);

            io_service.run();
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }

  return 0;

}