#include <network-monitor/websocket-client.h>

#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include <string>
#include <filesystem>


using NetworkMonitor::WebSocketClient;

BOOST_AUTO_TEST_SUITE(network_monitor);

BOOST_AUTO_TEST_CASE(class_WebSocketClient)
{
    // First of all, check if the TESTS_CACERT_PERM file is found, which contains CA certificates
    // for our SSL/TLS layer:

    BOOST_CHECK(std::filesystem::exists(TESTS_CACERT_PEM));
    
    // Connection targets
    const std::string url {"echo.websocket.org"};
    // change the port to use TLS
    const std::string port {"443"};
    const std::string message {"Hello WebSocket"};
    std::string echo {};

    // Declare a TLS context. Important to notice that we check the cacert_pem here
    boost::asio::ssl::context ctx {boost::asio::ssl::context::tlsv12_client};
    ctx.load_verify_file(TESTS_CACERT_PEM);

    // Always start with an I/O context object.
    boost::asio::io_context ioc {};

    // The class under test
    WebSocketClient client {url, port, ioc, ctx};

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
                      &message,
                      &echo](auto ec, auto received) {
        messageReceived = !ec;
        echo = std::move(received);
       client.Close(onClose);
    }};

    // We must call io_context::run for asynchronous callbacks to run.
    // we do not include the third argument, which is another function pointer
    // because we declared a default constructor argument to be nullptr this is fine
    client.Connect(onConnect, onReceive);
    ioc.run();

    // When we get here, the io_context::run function has run out of work to do.
    BOOST_CHECK(connected);
    BOOST_CHECK(messageSent);
    BOOST_CHECK(messageReceived);
    BOOST_CHECK(disconnected);
    BOOST_CHECK_EQUAL(message, echo);
}

BOOST_AUTO_TEST_SUITE_END();