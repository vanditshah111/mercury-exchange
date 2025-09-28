#include "MatchingEngine.hpp"
#include "MarketRegistry.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>

using namespace MercEx;

int main() {
    MarketRegistry registry;
    double tick = 0.5;

    // Create two markets
    MarketID aapl_id = 1, goog_id = 2;
    registry.create_market("AAPL", tick, aapl_id);
    registry.create_market("GOOG", tick, goog_id);

    MatchingEngine engine(registry);

    const int num_threads = 2;          // multiple producers
    const int orders_per_thread = 500000; // per thread
    const int total_orders = num_threads * orders_per_thread;

    std::vector<std::thread> producers;

    auto start = std::chrono::high_resolution_clock::now();

    for (int t = 0; t < num_threads; ++t) {
        producers.emplace_back([&, t]() {
            std::mt19937 rng(std::random_device{}() + t);
            std::uniform_int_distribution<int> side_dist(0, 1);
            std::uniform_int_distribution<int> symbol_dist(0, 1);
            std::uniform_int_distribution<int> qty_dist(10, 1000);
            std::uniform_int_distribution<int> type_dist(0, 2); // 0=Limit,1=Market,2=Stop

            for (int i = 0; i < orders_per_thread; ++i) {
                std::string symbol = (symbol_dist(rng) == 0) ? "AAPL" : "GOOG";
                Side side = (side_dist(rng) == 0) ? Side::Buy : Side::Sell;
                Quantity qty = qty_dist(rng);

                OrderType otype = static_cast<OrderType>(type_dist(rng));
                std::optional<Price> price;
                std::optional<Price> stop_price;

                if (otype == OrderType::Limit) {
                    // valid tick-aligned price
                    int ticks = std::uniform_int_distribution<int>(200, 400)(rng);
                    price = ticks * tick;
                } else if (otype == OrderType::Stop) {
                    // stop price also aligned to tick
                    int ticks = std::uniform_int_distribution<int>(200, 400)(rng);
                    stop_price = ticks * tick;
                }
                // Market order: no price or stop_price

                engine.submit_order(
                    t, symbol, qty, side, price, otype,
                    TimeInForce::GTC, stop_price
                );
            }
        });
    }

    for (auto& th : producers) th.join();

    auto end = std::chrono::high_resolution_clock::now();
    double total_ms = std::chrono::duration<double, std::milli>(end - start).count();

    // Print average latency from processors
    auto* procAAPL = registry.get_market_processor("AAPL");
    auto* procGOOG = registry.get_market_processor("GOOG");

    std::cout << "Submitted " << total_orders << " orders in " << total_ms << " ms\n";
    if (procAAPL) {
        std::cout << "AAPL average processing latency: "
                  << procAAPL->get_average_latency_ms() << " ms\n";
    }
    if (procGOOG) {
        std::cout << "GOOG average processing latency: "
                  << procGOOG->get_average_latency_ms() << " ms\n";
    }

    registry.print_markets();
    return 0;
}
