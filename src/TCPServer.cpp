#include "TCPServer.hpp"
#include "ClientSession.hpp"
#include <iostream>

TCPServer::TCPServer(boost::asio::io_context& io_context, short port,
                     MercEx::MatchingEngine& engine, MercEx::MarketDataPublisher& publisher)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      engine_(engine),
      publisher_(publisher) {
    std::cout << "Server listening on port " << port << std::endl;
    do_accept();
}

void TCPServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "Accepted new client connection." << std::endl;
                std::make_shared<ClientSession>(std::move(socket), engine_, publisher_)->start();
            }
            do_accept();
        });
}