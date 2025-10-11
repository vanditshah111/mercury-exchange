#include "GatewayProtocol.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <string>

using boost::asio::ip::tcp;
using namespace MercEx;

void listen_for_server_messages(tcp::socket& socket) {
    try {
        for (;;) {
            Gateway::MessageHeader header;
            boost::asio::read(socket, boost::asio::buffer(&header, sizeof(header)));

            std::vector<char> body(header.msg_size);
            boost::asio::read(socket, boost::asio::buffer(body));

            if (header.msg_type == Gateway::MessageType::NewOrderAck) {
                const auto* ack = reinterpret_cast<const Gateway::NewOrderAck*>(body.data());
                std::cout << "\n[Server] Order ACK: ID=" << ack->order_id
                          << " Symbol=" << ack->symbol << "\n> ";
            } else if (header.msg_type == Gateway::MessageType::TradeReport) {
                const auto* report = reinterpret_cast<const Gateway::TradeReport*>(body.data());
                std::cout << "\n[Server] TRADE: "
                          << report->quantity << " " << report->symbol
                          << " @ " << report->price << "\n> ";
            }
        }
    } catch (std::exception& e) {
        std::cout << "\n[Client] Connection closed or error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "8080");
        tcp::socket socket(io_context);

        boost::asio::connect(socket, endpoints);
        std::cout << "[Client] Connected to exchange server.\n";

        socket.set_option(boost::asio::ip::tcp::no_delay(true));

        std::thread listener(listen_for_server_messages, std::ref(socket));
        listener.detach();

        std::cout << "Enter orders in this format:\n";
        std::cout << "  SYMBOL SIDE PRICE QTY\n";
        std::cout << "Example:  AAPL BUY 150 100\n";
        std::cout << "Type 'exit' to quit.\n\n> ";

        long client_id = 1;
        long cl_ord_id = 100;

        std::string symbol, side_str;
        double price;
        long quantity;

        while (true) {
            std::string line;
            std::getline(std::cin, line);
            if (line.empty()) continue;
            if (line == "exit") break;

            std::stringstream ss(line);
            ss >> symbol >> side_str >> price >> quantity;

            if (ss.fail()) {
                std::cout << "Invalid format. Try again.\n> ";
                continue;
            }

            Gateway::NewOrderRequest req{};
            req.cl_ord_id = cl_ord_id++;
            req.client_id = client_id;
            std::strncpy(req.symbol, symbol.c_str(), sizeof(req.symbol) - 1);
            req.side = (side_str == "BUY" || side_str == "buy") ? Side::Buy : Side::Sell;
            req.order_type = OrderType::Limit;
            req.quantity = quantity;
            req.price = price;
            req.tif = TimeInForce::GTC;

            Gateway::MessageHeader header;
            header.msg_type = Gateway::MessageType::NewOrderRequest;
            header.msg_size = sizeof(req);

            boost::asio::write(socket, boost::asio::buffer(&header, sizeof(header)));
            boost::asio::write(socket, boost::asio::buffer(&req, sizeof(req)));

            std::cout << "[Client] Sent " << side_str << " order for " 
                      << quantity << " " << symbol << " @ " << price << "\n> ";
        }

        socket.close();
        std::cout << "\n[Client] Exiting.\n";

    } catch (std::exception& e) {
        std::cerr << "Main client exception: " << e.what() << "\n";
    }
    return 0;
}
