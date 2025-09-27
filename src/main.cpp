#include <iostream>
#include <thread>
#include <chrono>
#include "MarketRegistry.hpp"
#include "MatchingEngine.hpp"

using namespace MercEx;

int main() {
    // --- Setup ---
    MarketRegistry registry;
    MatchingEngine engine(registry);

    double tick = 0.5;
    uint16_t aapl_id = 1;
    uint16_t msft_id = 2;

    // Create two markets
    MarketProcessor& aapl_proc = registry.create_market("AAPL", tick, aapl_id);
    MarketProcessor& msft_proc = registry.create_market("MSFT", tick, msft_id);

    // Pause main thread for debugger if needed
    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.get();

    // --- Submit some orders ---
    engine.submit_order(101, "AAPL", 100, Side::Buy, 150.0, OrderType::Limit, TimeInForce::GTC);
    engine.submit_order(102, "AAPL", 50, Side::Sell, 149.5, OrderType::Limit, TimeInForce::GTC);
    engine.submit_order(103, "MSFT", 200, Side::Buy, std::nullopt, OrderType::Market, TimeInForce::IOC);
    engine.submit_order(104, "MSFT", 150, Side::Sell, 300.0, OrderType::Limit, TimeInForce::GTC);

    // --- Cancel example ---
    OrderID cancel_id = engine.submit_order(105, "AAPL", 30, Side::Sell, 151.0, OrderType::Limit, TimeInForce::GTC);
    engine.cancel_order(cancel_id, "AAPL");

    // Give some time for the processor threads to handle events
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cin.get();
    // Print the current market state
    registry.print_markets();

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    // Stop all processors gracefully
    registry.remove_market("AAPL");
    registry.remove_market("MSFT");

    return 0;
}
