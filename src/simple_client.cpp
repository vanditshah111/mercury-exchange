#include "GatewayProtocol.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;
using namespace MercEx;
std::chrono::high_resolution_clock::time_point start1,start2;
std::chrono::high_resolution_clock::time_point end;
// This function runs on a separate thread to continuously listen for server messages
void listen_for_server_messages(tcp::socket& socket) {
    try {
        for (;;) {
            Gateway::MessageHeader header;
            boost::asio::read(socket, boost::asio::buffer(&header, sizeof(header)));

            std::vector<char> body(header.msg_size);
            boost::asio::read(socket, boost::asio::buffer(body));
            if (header.msg_type == Gateway::MessageType::NewOrderAck) {
                const auto* ack = reinterpret_cast<const Gateway::NewOrderAck*>(body.data());
                end = std::chrono::high_resolution_clock::now();
                if(ack->client_id==1)
                {
                    auto duration = std::chrono::duration<double,std::micro>(end - start1).count();
                    std::cout << "[Client] Round-trip time for order ID " << ack->order_id << ": " << duration << " milliseconds" << std::endl;
                }
                else if(ack->client_id==2)
                {
                    auto duration = std::chrono::duration<double,std::micro>(end - start2).count();
                    std::cout << "[Client] Round-trip time for order ID " << ack->order_id << ": " << duration << " milliseconds" << std::endl;
                }
                std::cout << "[Client] Received NewOrderAck: "
                          << ack->cl_ord_id << " for " << ack->symbol << std::endl;
            } else if (header.msg_type == Gateway::MessageType::TradeReport) {
                const auto* report = reinterpret_cast<const Gateway::TradeReport*>(body.data());
                end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double,std::micro>(end - start2).count();
                std::cout << "[Client] Round-trip time for trade " << ": " << duration << " milliseconds" << std::endl;
                std::cout << "[Client] Received Trade: "
                          << report->quantity << " of " << report->symbol
                          << " @ " << report->price << std::endl;
            }
            // TODO: Add handlers for ExecutionReports
        }
    } catch (std::exception& e) {
        std::cout << "[Client] Connection closed or error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "8080");
        tcp::socket socket(io_context);

        boost::asio::connect(socket, endpoints);
        std::cout << "[Client] Connected to exchange server." << std::endl;
        socket.set_option(boost::asio::ip::tcp::no_delay(true));
        std::thread listener(listen_for_server_messages, std::ref(socket));
        listener.detach();

        // --- Send a BUY order ---
        Gateway::NewOrderRequest buy_req{}; // Zero-initialize
        buy_req.cl_ord_id = 101;
        buy_req.client_id = 1;
        std::strncpy(buy_req.symbol, "AAPL", sizeof(buy_req.symbol) - 1);
        buy_req.side = Side::Buy;
        buy_req.order_type = OrderType::Limit;
        buy_req.quantity = 100;
        buy_req.price = 150.0;
        buy_req.tif = TimeInForce::GTC;

        Gateway::MessageHeader buy_header;
        buy_header.msg_type = Gateway::MessageType::NewOrderRequest;
        buy_header.msg_size = sizeof(buy_req);

        std::cout << "[Client] Sending BUY order for 100 AAPL @ 150.0" << std::endl;
        start1 = std::chrono::high_resolution_clock::now();
        boost::asio::write(socket, boost::asio::buffer(&buy_header, sizeof(buy_header)));
        boost::asio::write(socket, boost::asio::buffer(&buy_req, sizeof(buy_req)));

        // --- Send a matching SELL order ---
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        Gateway::NewOrderRequest sell_req{}; // Zero-initialize
        sell_req.cl_ord_id = 102;
        sell_req.client_id = 2;
        std::strncpy(sell_req.symbol, "GOOG", sizeof(sell_req.symbol) - 1);
        sell_req.side = Side::Sell;
        sell_req.order_type = OrderType::Limit;
        sell_req.quantity = 50;
        sell_req.price = 150.0;
        sell_req.tif = TimeInForce::GTC;

        Gateway::MessageHeader sell_header;
        sell_header.msg_type = Gateway::MessageType::NewOrderRequest;
        sell_header.msg_size = sizeof(sell_req);
        
        std::cout << "[Client] Sending SELL order for 50 AAPL @ 150.0" << std::endl;
        start2 = std::chrono::high_resolution_clock::now();
        boost::asio::write(socket, boost::asio::buffer(&sell_header, sizeof(sell_header)));
        boost::asio::write(socket, boost::asio::buffer(&sell_req, sizeof(sell_req)));

        // Keep the main thread alive to see the response
        std::this_thread::sleep_for(std::chrono::seconds(5));

    } catch (std::exception& e) {
        std::cerr << "Main client exception: " << e.what() << "\n";
    }
    return 0;
}