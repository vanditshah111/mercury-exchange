#include <iostream>
#include "Order.hpp"
#include "Market.hpp"
#include "MarketRegistry.hpp"
#include "MatchingEngine.hpp"

using namespace MercEx;

int main() {
    try {
        MarketRegistry registry;
        MatchingEngine engine(registry);

        // Create market
        Market& market = registry.create_market("AAPL", 0.01);

        // === STEP 1: Buy Limit ===
        Order buy1 = Order::make_limit_order(1, 101, "AAPL", 50, 100.0, Side::Buy, TimeInForce::GTC);
        std::cout << "Submitting Buy Order:\n" << to_string(buy1) << "\n";
        engine.match_order(buy1);
        market.print_order_books();

        // === STEP 2: Sell Limit ===
        Order sell1 = Order::make_limit_order(2, 201, "AAPL", 70, 101.0, Side::Sell, TimeInForce::GTC);
        std::cout << "\nSubmitting Sell Order:\n" << to_string(sell1) << "\n";
        engine.match_order(sell1);
        market.print_order_books();

        // === STEP 3: Aggressive Buy Limit (crosses @101) ===
        Order buy2 = Order::make_limit_order(3, 102, "AAPL", 80, 105.0, Side::Buy, TimeInForce::GTC);
        std::cout << "\nSubmitting Aggressive Buy Order:\n" << to_string(buy2) << "\n";
        engine.match_order(buy2);
        market.print_order_books();

        // === STEP 4: Market Sell Order (consumes best bid) ===
        Order sell2 = Order::make_market_order(4, 202, "AAPL", 30, Side::Sell, TimeInForce::IOC);
        std::cout << "\nSubmitting Market Sell Order:\n" << to_string(sell2) << "\n";
        engine.match_order(sell2);
        market.print_order_books();

        // === Cancel test ===
        std::cout << "\nCancelling order ID=2...\n";
        if (engine.cancel_order(2)) {
            std::cout << "Order cancelled successfully\n";
        } else {
            std::cout << "Cancel failed (order already matched/removed)\n";
        }
        market.print_order_books();

        // === Market snapshot ===
        std::cout << "\n=== Market Registry ===\n";
        registry.print_markets();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}
