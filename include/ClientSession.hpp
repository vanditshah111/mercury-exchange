#pragma once
#include "MatchingEngine.hpp"
#include "MarketDataPublisher.hpp"
#include "IMarketDataListener.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <deque>
#include <string>
#include <iostream>

using boost::asio::ip::tcp;

class ClientSession : public MercEx::IMarketDataListener, public std::enable_shared_from_this<ClientSession> {
public:
    ClientSession(tcp::socket socket, MercEx::MatchingEngine& engine, MercEx::MarketDataPublisher& publisher);
    ~ClientSession();

    void start();

    // This method is called by the MarketDataPublisher's thread
    void on_market_events(const std::vector<MercEx::MarketEvent>& events) override;

private:
    void do_read();
    void do_write();

    tcp::socket socket_;
    MercEx::MatchingEngine& engine_;
    MercEx::MarketDataPublisher& publisher_;

    enum { max_length = 1024 };
    char read_data_[max_length];

    // A thread-safe queue for outgoing messages
    std::deque<std::string> write_msgs_;
};