#pragma once
#include "MatchingEngine.hpp"
#include "MarketDataPublisher.hpp"
#include "ClientSession.hpp"

using boost::asio::ip::tcp;

class TCPServer {
public:
    TCPServer(boost::asio::io_context& io_context, short port,
              MercEx::MatchingEngine& engine, MercEx::MarketDataPublisher& publisher);

private:
    void do_accept();

    tcp::acceptor acceptor_;
    MercEx::MatchingEngine& engine_;
    MercEx::MarketDataPublisher& publisher_;
};