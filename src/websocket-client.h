#include <iostream>
#include <functional>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/system/error_code.hpp>

namespace NetworkMonitor {


class WebSocketClient {
public:
    /*! \brief Construct a WebSocket client.
     *
     *  \note This constructor does not initiate a connection.
     *
     *  \param url  The URL of the server.
     *  \param port The port on the server.
     *  \param ioc  The io_context object. The user takes care of calling
     *              ioc.run().
     */
    WebSocketClient(
        const std::string& url,
        const std::string& port,
        boost::asio::io_context& ioc
    );

    /*! \brief Destructor.
     */
    ~WebSocketClient();

    /*! \brief Connect to the server.
     *
     *  \param onConnect     Called when the connection fails or succeeds.
     *  \param onMessage     Called only when a message is successfully
     *                       received. The message is an rvalue reference;
     *                       ownership is passed to the receiver.
     *  \param onDisconnect  Called when the connection is closed by the server
     *                       or due to a connection error.
     */
    void Connect(
        std::function<void (boost::system::error_code)> onConnect = nullptr,
        std::function<void (boost::system::error_code,
                            std::string&&)> onMessage = nullptr,
        std::function<void (boost::system::error_code)> onDisconnect = nullptr
    );

    /*! \brief Send a text message to the WebSocket server.
     *
     *  \param message The message to send. The caller must ensure that this
     *                 string stays in scope until the onSend handler is called.
     *  \param onSend  Called when a message is sent successfully or if it
     *                 failed to send.
     */
    void Send(
        const std::string& message,
        std::function<void (boost::system::error_code)> onSend = nullptr
    );

    /*! \brief Close the WebSocket connection.
     *
     *  \param onClose Called when the connection is closed, successfully or
     *                 not.
     */
    void Close(
        std::function<void (boost::system::error_code)> onClose = nullptr
    );

private:
    std::string url_ {};
    std::string port_ {};

    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;

};

} 