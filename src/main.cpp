#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/system/error_code.hpp>

#include <iomanip>
#include <iostream>
#include <string>


using tcp = boost::asio::ip::tcp;
namespace websocket = boost::beast::websocket;

void Log(const std::string& where, boost::system::error_code ec)
{
    std::cerr << "[" << std::setw(20) << where << "] "
              << (ec ? "Error: " : "OK")
              << (ec ? ec.message() : "")
              << std::endl;
}



int main() {
    
    
    boost::asio::io_context ioc {};
    tcp::socket socket {boost::asio::make_strand(ioc)};
    tcp::resolver resolver {ioc};

    std::string url {"echo.websocket.org"};
    std::string port {"80"};
    boost::system::error_code ec {};


    auto resolverIt {resolver.resolve(url,port, ec)};

    socket.connect(*resolverIt, ec);

    //After the socket.connect call succeeds, we are ready to perform the WebSocket handshake
    // we create the websocket connection



    // Tie the socket object to the WebSocket stream and attempt an handshake.
    websocket::stream<boost::beast::tcp_stream> ws {std::move(socket)};
    ws.handshake(url, "/", ec);
    if (ec) {
        Log("ws.handshake", ec);
        return -3;
    }

    // Tell the WebSocket object to exchange messages in text format.
    std::string message {"Hello World!"};
    ws.text(true);

    // Send a message to the connected WebSocket server.
    boost::asio::const_buffer wbuffer {message.c_str(), message.size()};
    ws.write(wbuffer, ec);
     if (ec) {
        Log("ws.write", ec);
        return -4;
    }

    // Read the echoed message back.
    boost::beast::flat_buffer rbuffer {};
    ws.read(rbuffer, ec);

    // Print the echoed message.
        std::cout << "ECHO: "
          << boost::beast::make_printable(rbuffer.data())
          << std::endl;

}