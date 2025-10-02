#include "MatchingEngine.hpp"
#include "MarketRegistry.hpp"
#include <iostream>
#include <cassert>
#include <thread>

using namespace MercEx;

int main() {
    std::cout << "--- Running Test: Full Order Match ---" << std::endl;
    MarketRegistry registry;
    MatchingEngine engine(registry);
    registry.create_market("TEST", 0.01, 1);

    // Submit a buy order
    engine.submit_order(1, "TEST", 10, Side::Buy, 100.00, OrderType::Limit, TimeInForce::GTC);
    // Submit a sell order that perfectly matches
    engine.submit_order(2, "TEST", 10, Side::Sell, 100.00, OrderType::Limit, TimeInForce::GTC);

    std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Allow processing

    Market& market = registry.get_market_processor("TEST")->get_market();
    assert(market.get_last_price().value() == 100.00);
    assert(market.get_buybook().empty());
    assert(market.get_sellbook().empty());

    std::cout << "[PASS] Books are empty and last price is correct." << std::endl;
}