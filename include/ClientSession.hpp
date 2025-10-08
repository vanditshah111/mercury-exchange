#pragma once
#include "MatchingEngine.hpp"
#include "MarketDataPublisher.hpp"
#include "IMarketDataListener.hpp"
#include "GatewayProtocol.hpp" // <-- Add this
#include <boost/asio.hpp>
#include <memory>
#include <deque>
#include <string>
#include <vector> // <-- Add this
#include <iostream>

using boost::asio::ip::tcp;

class ClientSession : public MercEx::IMarketDataListener, public std::enable_shared_from_this<ClientSession> {
public:
    ClientSession(tcp::socket socket, MercEx::MatchingEngine& engine, MercEx::MarketDataPublisher& publisher);
    ~ClientSession();

    void start();
    void on_market_events(const std::vector<MercEx::MarketEvent>& events) override;

private:
    void do_read_header();
    void do_read_body();
    void process_binary_message();
    void do_write();

    tcp::socket socket_;
    MercEx::MatchingEngine& engine_;
    MercEx::MarketDataPublisher& publisher_;

    // Buffers for binary reading
    MercEx::Gateway::MessageHeader read_header_;
    std::vector<char> read_body_;

    // Queue for binary writing
    std::deque<std::vector<char>> write_msgs_;
};