#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

using tcp = boost::asio::ip::tcp;

void Log(boost::system::error_code ec){
    std::cerr   << (ec ? "Error: " : "OK")
                << (ec ? ec.message() : "")
                << std::endl;
}



int main() {
    
    // Always start with an IO Context: intermediary between the different I/O interfaces
    // and get a result back
    boost::asio::io_context ioc {};

    // Create an I/O object, which needs a context:
    tcp::socket socket {ioc};

    // Under the hood, socket.connect uses I/O COntext to talk to the socket and get a response back. 
    // response is saved in ec
    boost::system::error_code ec {};

    // Connects to the DNS "1.1.1.1"
    //auto address {boost::asio::ip::address::from_string("1.1.1.1")};
    //tcp::endpoint endpoint {address, 80};

    // We want to access to an url and let the DNS resolve it:
    tcp::resolver resolver {ioc};
    auto resolverIt {resolver.resolve("google.com", "80", ec)};

    

    socket.connect(endpoint, ec);

    if (ec){
        std::cerr << "Error: " << ec.message() << std::endl;
        return -1;
    } else {
        std::cout << "OK" <<std::endl;
        return 0;
    }
}