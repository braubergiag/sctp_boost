#include <boost/asio.hpp>

#include <arpa/inet.h>
#include <iostream>
#include <linux/sctp.h>
#include <boost/array.hpp>
#include <netinet/in.h>
namespace asio = boost::asio;








int main(int argc, char * argv[]){


    if (argc < 2){
        exit(0);
    }


 try
  {


    boost::asio::io_context io_context;
    unsigned short MY_PORT_NUM = atoi(argv[1]);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(MY_PORT_NUM);
    asio::io_service ios;
    using protocol = asio::generic::stream_protocol;
    protocol::endpoint ep{
        reinterpret_cast<struct sockaddr *>(&servaddr),
        sizeof(servaddr),
        IPPROTO_SCTP
    };
   protocol::socket socket(ios);

   boost::system::error_code  ec;
    socket.connect(ep,ec);
    if (!ec){
      std::cout << "connect successfuly\n";
    }


    asio::socket_base::message_flags in_flags;
    asio::socket_base::message_flags out_flags;


    for (;;)
    {
      boost::array<char, 128> buffer;
      boost::system::error_code error;

      size_t len = socket.read_some(boost::asio::buffer(buffer), error);

      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      std::cout.write(buffer.data(), len);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }


    return 0;
}