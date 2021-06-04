#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <thread>
#include <iomanip>

using tcp = boost::asio::ip::tcp;

void Log(boost::system::error_code ec){
    std::cerr   << (ec ? "Error: " : "OK")
                << (ec ? ec.message() : "")
                << std::endl;
}

void OnConnect(boost::system::error_code ec)
{

Log(ec);

}



int main() {
    
    // 
    std::cerr << "[" << std::setw(14) << std::this_thread::get_id() << "] main" << std::endl;

    // Always start with an IO Context: intermediary between the different I/O interfaces
    // and get a result back
    boost::asio::io_context ioc {};

    // Create an I/O object, which needs a context:
    tcp::socket socket {boost::asio::make_strand(ioc)};

    // Under the hood, socket.connect uses I/O COntext to talk to the socket and get a response back. 
    // response is saved in ec
    boost::system::error_code ec {};

    // Connects to the DNS "1.1.1.1"
    //auto address {boost::asio::ip::address::from_string("1.1.1.1")};
    //tcp::endpoint endpoint {address, 80};

    // We want to access to an url and let the DNS resolve it:
    tcp::resolver resolver {ioc};
    auto resolverIt {resolver.resolve("google.com", "80", ec)};

    if (ec){
        std::cerr << "Error: " << ec.message() << std::endl;
        return -1;
    } 
    
    size_t nThreads {4};
    /* Async connection
    * @{Nick}
    */

   for (size_t idx {0}; idx < nThreads; ++idx){
       // For each thread we will call async_connect 4 times. This will be done using io_context::run on a separate thread
       socket.async_connect(*resolverIt, OnConnect);
   }

   std::vector<std::thread> threads{};

   threads.reserve(4);
   for (size_t idx {0}; idx < nThreads; ++idx){
       threads.emplace_back(
           [&ioc](){
            std::cerr << "[" << std::setw(14) << std::this_thread::get_id() << "] ioc.run()" << std::endl;
            ioc.run();
        }
       );
   }

    // we must call io_context::run for asynchronous callbacks to run.
    // we can run the io context in a different thread
    std::thread thread {
    };

    thread.join();
}