#pragma once
#include "Market.hpp"
#include "MarketEvent.hpp"
#include <deque>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <list>
#include <map>
#include <memory>
#include <thread>
#include <atomic>

namespace MercEx {

class MarketProcessor {
public:
    explicit MarketProcessor(std::unique_ptr<Market> m);
    ~MarketProcessor();

    void submit_event(const MarketEvent& ev);
    void run();
    void stop();
    Market& get_market();
    MarketID get_market_id() const;

private:
    void handle_event(MarketEvent& ev);
    void check_stop_orders();
    void handle_market_events(const std::vector<MarketEvent>& events);

    std::unique_ptr<Market> market;

    std::deque<MarketEvent> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool running;
    std::thread worker;
    std::unordered_map<OrderID, std::unique_ptr<Order>> orders_local_;

    std::map<Price, std::list<Order*>, std::greater<>> stop_buy_orders_;
    std::map<Price, std::list<Order*>, std::less<>> stop_sell_orders_;
};

} // namespace MercEx
