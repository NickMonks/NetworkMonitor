#include <network-monitor/websocket-client.h>

#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include <string>
#include <filesystem>


using NetworkMonitor::WebSocketClient;


bool CheckResponse(const std::string& response)
{
    // We do not parse the whole message. We only check that it contains some
    // expected items.
    bool ok {true};
    ok &= response.find("ERROR") != std::string::npos;
    ok &= response.find("ValidationInvalidAuth") != std::string::npos;
    return ok;
}

BOOST_AUTO_TEST_CASE(cacert_pem)
{
    BOOST_CHECK(std::filesystem::exists(TESTS_CACERT_PEM));
}

BOOST_AUTO_TEST_SUITE(network_monitor);

BOOST_AUTO_TEST_CASE(send_stomp)
{
    const std::string url {"ltnm.learncppthroughprojects.com"};
    const std::string endpoint {"/network-events"};
    const std::string port {"443"};

    // create a buffer stream where we can send a STOMP message:
    const std::string username {"fake_username"};
    const std::string password {"fake_username"};

    std::stringstream ss {};

    ss << "STOMP" << std::endl
    << "accept-version:1.2" << std::endl
    << "host:transportforlondon.com" <<std::endl
    << "login:"<<username<<std::endl
    << "passcode:" << password<<std::endl
    << std::endl // headers need to be followed by a blank line
    << '\0'; // NULL Octet

    const std::string message {ss.str()};

    // TLS context
    boost::asio::ssl::context ctx {boost::asio::ssl::context::tlsv12_client};
    ctx.load_verify_file(TESTS_CACERT_PEM);

    // Always start with an I/O context object.
    boost::asio::io_context ioc {};

    // The class under test
    WebSocketClient client {url, endpoint, port, ioc, ctx};

    // We use these flags to check that the connection, send, receive functions
    // work as expected.
    bool connected {false};
    bool messageSent {false};
    bool messageReceived {false};
    bool disconnected {false};
    std::string response {};

    // Our own callbacks
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
    auto onReceive {[&client,
                     &onClose,
                     &messageReceived,
                     &response](auto ec, auto received) {
        messageReceived = !ec;
        response = std::move(received);
        client.Close(onClose);
    }};

    // We must call io_context::run for asynchronous callbacks to run.
    client.Connect(onConnect, onReceive);
    ioc.run();

    // Check the response of the message from server using the CheckResponse function
    BOOST_CHECK(connected);
    BOOST_CHECK(messageSent);
    BOOST_CHECK(messageReceived);
    BOOST_CHECK(disconnected);
    BOOST_CHECK(CheckResponse(response));
}

BOOST_AUTO_TEST_CASE(class_WebSocketClient)
{
    // First of all, check if the TESTS_CACERT_PERM file is found, which contains CA certificates
    // for our SSL/TLS layer:

    BOOST_CHECK(std::filesystem::exists(TESTS_CACERT_PEM));
    
    // Connection targets
    const std::string url {"echo.websocket.org"};
    // change the port to use TLS
    const std::string endpoint {"/"};

    const std::string port {"443"};
    const std::string message {"Hello WebSocket"};
    std::string echo {};

    // Declare a TLS context. Important to notice that we check the cacert_pem here
    boost::asio::ssl::context ctx {boost::asio::ssl::context::tlsv12_client};
    ctx.load_verify_file(TESTS_CACERT_PEM);

    // Always start with an I/O context object.
    boost::asio::io_context ioc {};

    // The class under test
    WebSocketClient client {url, endpoint, port, ioc, ctx};

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