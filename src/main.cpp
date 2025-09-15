#include "MarketRegistry.hpp"
#include "MatchingEngine.hpp"
#include <iostream>

using namespace MercEx;

int main() {
    // --- Setup ---
    MarketRegistry registry;

    // Add markets to the registry
    registry.create_market("AAPL", 0.01);
    registry.create_market("GOOG", 0.01);

    // Create matching engine
    MatchingEngine engine(registry);

    // --- Submit some orders ---
    try {
        // Limit orders
        Market* market = registry.get_market("AAPL");
        OrderID b1 = engine.submit_order(1, "AAPL", 100, Side::Buy, 150.00, OrderType::Limit, TimeInForce::GTC);
        OrderID b2 = engine.submit_order(2, "AAPL", 50,  Side::Buy, 149.50, OrderType::Limit, TimeInForce::GTC);
        OrderID s1 = engine.submit_order(3, "AAPL", 80,  Side::Sell, 150.00, OrderType::Limit, TimeInForce::GTC);
        market->print_order_books();
        // Market order (should match immediately)
        OrderID b3 = engine.submit_order(4, "AAPL", 60,  Side::Buy, std::nullopt, OrderType::Market, TimeInForce::IOC);

        // Cancel one order
        bool cancelled = engine.cancel_order(b2);
        std::cout << "Cancelled order b2: " << std::boolalpha << cancelled << "\n";
        cancelled = engine.cancel_order(b3);
        std::cout << "Cancelled order b2: " << std::boolalpha << cancelled << "\n";
        // --- Print order books ---
        
        if (market) {
            market->print_order_books();
        } else {
            std::cerr << "Market not found!\n";
        }

        // --- Inspect an order by ID ---
        const Order* ord = engine.get_order(b1);
        if (ord) {
            std::cout << "Order " << ord->id 
                      << " has remaining " << ord->remaining 
                      << " and status " << to_string(ord->status) << "\n";
        }

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}
