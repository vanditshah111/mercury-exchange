#include <iostream>
#include "Order.hpp"
#include "Market.hpp"
#include "BuyBook.hpp"
#include "SellBook.hpp"
#include "MarketRegistry.hpp"
#include "MatchingEngine.hpp"
#include <cmath>

using namespace MercEx;

int main() {
    try {
        MarketRegistry registry;
        MatchingEngine engine(registry);

        // Create a market
        Market& market = registry.create_market("AAPL", 0.01);
        std::cout << "Created market: " << market.get_symbol() << "\n";

        // Place a SELL order (resting)
        Order sell1 = Order::make_limit_order(1, 1001, "AAPL", 10, 150.0, Side::Sell, TimeInForce::GTC);
        std::cout << "Placing sell order: " << to_string(sell1) << "\n";
        engine.match_order(sell1);

        // Place a BUY order that crosses -> should match fully
        Order buy1 = Order::make_limit_order(2, 1002, "AAPL", 10, 151.0, Side::Buy, TimeInForce::GTC);
        std::cout << "Placing buy order: " << to_string(buy1) << "\n";
        engine.match_order(buy1);

        registry.print_markets();

        // Place another SELL order (will rest partially)
        Order sell2 = Order::make_limit_order(3, 1003, "AAPL", 15, 152.0, Side::Sell, TimeInForce::GTC);
        std::cout << "Placing sell order: " << to_string(sell2) << "\n";
        engine.match_order(sell2);

        // Place a BUY order smaller than sell2 -> partial fill
        Order buy2 = Order::make_limit_order(4, 1004, "AAPL", 5, 152.0, Side::Buy, TimeInForce::GTC);
        std::cout << "Placing buy order: " << to_string(buy2) << "\n";
        engine.match_order(buy2);

        registry.print_markets();

        // Cancel the remaining sell2
        bool cancelled = engine.cancel_order(sell2.id);
        std::cout << "Cancel order " << sell2.id << (cancelled ? " succeeded" : " failed") << "\n";

        registry.print_markets();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
