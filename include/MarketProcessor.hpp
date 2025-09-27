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
#include <iostream>
namespace MercEx
{

    class MarketProcessor
    {
    public:
        explicit MarketProcessor(std::unique_ptr<Market> m);
        ~MarketProcessor();

        void submit_event(const MarketEvent &ev);
        void run();
        void stop();
        Market &get_market();
        MarketID get_market_id() const;
        double get_average_latency_ms() const
        {
            std::cout << processed_orders << std::endl;
            if (processed_orders == 0)
                return 0.0;
            return (total_latency_ns / 1e6) / processed_orders; // convert ns → ms
        }

    private:
        void handle_event(MarketEvent &ev);
        void check_stop_orders();
        void handle_market_events(const std::vector<MarketEvent> &events);

        std::unique_ptr<Market> market;

        std::deque<MarketEvent> queue;
        std::mutex mtx;
        std::condition_variable cv;
        bool running;
        std::thread worker;
        std::unordered_map<OrderID, std::unique_ptr<Order>> orders_local_;

        uint64_t total_latency_ns = 0;
        size_t processed_orders = 0;

        std::map<Price, std::list<Order *>, std::greater<>> stop_buy_orders_;
        std::map<Price, std::list<Order *>, std::less<>> stop_sell_orders_;
    };

} // namespace MercEx
