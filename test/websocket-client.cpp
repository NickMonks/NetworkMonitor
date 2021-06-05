#include "websocket-client.h"

#include <boost/asio.hpp>

#include <iostream>
#include <string>

using NetworkMonitor::WebSocketClient;

int main()
{
    // Connection targets
    const std::string url {"echo.websocket.org"};
    const std::string port {"80"};
    const std::string message {"Hello WebSocket"};

    // Always start with an I/O context object.
    boost::asio::io_context ioc {};

    // The class under test
    WebSocketClient client {url, port, ioc};

    // We use these flags to check that the connection, send, receive functions
    // work as expected.
    bool connected {false};
    bool messageSent {false};
    bool messageReceived {false};
    bool messageMatches {false};
    bool disconnected {false};

    // Our own callbacks - this returns the flags value 
    auto onSend {[&messageSent](auto ec) {
        messageSent = !ec;
    }};
    auto onConnect {[&client, &connected, &onSend, &message](auto ec) {
        connected = !ec;
        if (!ec) {
            client.Send(message, onSend);
        }
    }};
    auto onClose {[&disconnected](auto ec) {
        disconnected = !ec;
    }};

    // lambda function that checks if message was sent correctly
    auto onReceive {[&client,
                      &onClose,
                      &messageReceived,
                      &messageMatches,
                      &message](auto ec, auto received) {
        messageReceived = !ec;
        messageMatches = message == received;
        client.Close(onClose);
    }};

    // We must call io_context::run for asynchronous callbacks to run.
    // we do not include the third argument, which is another function pointer
    // because we declared a default constructor argument to be nullptr this is fine
    client.Connect(onConnect, onReceive);
    ioc.run();

    // When we get here, the io_context::run function has run out of work to do.
    bool ok {
        connected &&
        messageSent &&
        messageReceived &&
        messageMatches &&
        disconnected
    };
    if (ok) {
        std::cout << "OK" << std::endl;
        return 0;
    } else {
        std::cerr << "Test failed" << std::endl;
        return 1;
    }
}