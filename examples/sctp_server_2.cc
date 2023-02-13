#include <boost/asio.hpp>

#include <arpa/inet.h>
#include <iostream>
#include <linux/sctp.h>
#include <netinet/in.h>
namespace asio = boost::asio;
using boost::system::error_code;


std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}


int main(int argc, char* argv[]) {
    std::vector args(argv + 1, argv + argc);


    uint16_t port = atoi(args.at(0));
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);

    using protocol = asio::generic::stream_protocol;
    protocol::endpoint ep{reinterpret_cast<struct sockaddr*>(&servaddr),
        sizeof(servaddr),
        IPPROTO_SCTP};

        asio::io_context ioc;
        asio::basic_socket_acceptor<protocol> acceptor(ioc);

    error_code ec;
    if (!ec)
        acceptor.open({AF_INET,IPPROTO_SCTP}, ec);
    if (!ec)
        acceptor.bind(ep,ec);
    if (!ec)
        acceptor.listen(protocol::socket::max_listen_connections,ec);
    const char *msg = "Hello, Server!";
    
    if (!ec){
        for (;;){
        protocol::socket socket(ioc);
        acceptor.accept(socket,ec);
        std::string message = make_daytime_string();
        boost::system::error_code ignored_error;
        boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
        }
    }


    std::cout << ec.message() << "\n";



    return 0;
}