#include "MarketRegistry.hpp"
#include "MatchingEngine.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>

using namespace MercEx;

int main() {
    MarketRegistry registry;
    MatchingEngine engine(registry);

    // Create markets
    double tick = 0.5;
    auto& aapl_proc = registry.create_market("AAPL", tick, 1);
    auto& goog_proc = registry.create_market("GOOG", tick, 2);

    std::vector<std::string> symbols = {"AAPL", "GOOG"};
    std::vector<Side> sides = {Side::Buy, Side::Sell};

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> qty_dist(10, 100);
    std::uniform_int_distribution<int> price_dist(140, 160);
    std::uniform_real_distribution<double> stop_offset(1.0, 3.0);
    std::uniform_int_distribution<int> type_dist(0, 2); // Limit, Market, Stop

    const int num_orders = 1000000;
    std::vector<OrderID> order_ids;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_orders; ++i) {
        std::string sym = symbols[i % symbols.size()];
        Side side = sides[i % sides.size()];
        Quantity qty = qty_dist(rng);

        OrderType type;
        std::optional<Price> price;
        std::optional<Price> stop_price;

        int t = type_dist(rng);
        if (t == 0) { // Limit
            type = OrderType::Limit;
            price = price_dist(rng);
        } else if (t == 1) { // Market
            type = OrderType::Market;
        } else { // Stop
            type = OrderType::Stop;
            price = std::nullopt;
            double last = price_dist(rng);
            stop_price = (side == Side::Buy) ? last + stop_offset(rng)
                                             : last - stop_offset(rng);
        }

        OrderID id = engine.submit_order(i, sym, qty, side, price, type, TimeInForce::GTC, stop_price);
        order_ids.push_back(id);
    }

    // Give processor threads some time to finish
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto end = std::chrono::high_resolution_clock::now();
    double total_ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "Submitted " << num_orders << " orders.\n";
    std::cout << "Total submission + processing time: " << total_ms << " ms\n";

    // Print per-market average processing latency
    for (const auto& sym : symbols) {
        auto* proc = registry.get_market_processor(sym);
        if (proc) {
            std::cout << sym << " average processing latency: "
                      << proc->get_average_latency_ms() << " ms\n";
        }
    }

    registry.remove_market("AAPL");
    registry.remove_market("GOOG");

    return 0;
}
