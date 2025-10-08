#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io, const std::string& host, int port)
        : socket_(io)
    {
        tcp::resolver resolver(io);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        boost::asio::connect(socket_, endpoints);

        std::cout << "Connected to server at " << host << ":" << port << "\n";
        start_read();
        start_write();
    }

private:
    void start_read() {
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    // ✅ SAFE extraction — avoids MSVC debug iterator issues
                    std::string line(
                        boost::asio::buffers_begin(buffer_.data()),
                        boost::asio::buffers_begin(buffer_.data()) + length
                    );
                    buffer_.consume(length);

                    // Trim newlines
                    while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
                        line.pop_back();

                    std::cout << "[Server] " << line << "\n";
                    start_read(); // continue reading
                } else if (ec == boost::asio::error::eof) {
                    std::cout << "[Client] Connection closed by server.\n";
                } else {
                    std::cerr << "[Client] Read error: " << ec.message() << "\n";
                }
            });
    }

    void start_write() {
        std::thread([this]() {
            std::string msg;
            while (std::getline(std::cin, msg)) {
                msg += '\n';
                boost::asio::write(socket_, boost::asio::buffer(msg));
            }
        }).detach();
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
};

int main() {
    try {
        boost::asio::io_context io;
        Client client(io, "127.0.0.1", 8080);
        io.run();
    } catch (std::exception& e) {
        std::cerr << "Client exception: " << e.what() << "\n";
    }
}
