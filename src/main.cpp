#include <iostream>
#include "Order.hpp"
#include "Market.hpp"
#include "BuyBook.hpp"
#include "SellBook.hpp"
#include "MarketRegistry.hpp"
#include "MatchingEngine.hpp"
#include <cmath>

int main() {
    using namespace MercEx;

    try {
        MarketRegistry registry;

        // Create market with tick size 0.01
        Market& mkt = registry.create_market("AAPL", 0.01);
        MatchingEngine mtche(registry);
        // Seed the SELL book (resting asks)
        Order ask1 = Order::make_limit_order(101, 2001, "AAPL", 70, 100.00, Side::Sell, TimeInForce::GTC); // 70 @ 100
        Order ask2 = Order::make_limit_order(102, 2002, "AAPL", 50, 102.00, Side::Sell, TimeInForce::GTC); // 50 @ 102
        mtche.match_order(ask1); // will rest on the book
        mtche.match_order(ask2); // will rest on the book
        registry.create_market("GOOG", 0.01);
        registry.get_market("AAPL")->deactivate(); 
        registry.get_market("AAPL")->activate(); // another market to show registry handling
        std::cout << "\n== After seeding asks ==\n";
        registry.print_markets();

        // Cross the spread: BUY 60 @ 120 → should match 60 against 100.00
        Order buy1 = Order::make_limit_order(201, 1001, "AAPL", 60, 120.00, Side::Buy, TimeInForce::GTC);
        mtche.match_order(buy1); // will reduce resting asks
        std::cout << "\nExecuted BUY 60 @ 120 (GTC). Remaining on order: " << buy1.remaining << "\n";
        registry.print_markets();

        // IOC example: BUY 80 @ 101 — will take whatever is available up to 101, remainder cancelled
        Order buyIOC = Order::make_limit_order(202, 1002, "AAPL", 80, 101.00, Side::Buy, TimeInForce::IOC);
        mtche.match_order(buyIOC);
        std::cout << "\nExecuted BUY 80 @ 101 (IOC). Remaining (should be 0, leftover cancelled): "
                  << buyIOC.remaining << "\n";
        registry.print_markets();

        // FOK example: require full fill; if not possible, reject
        try {
            Order buyFOK = Order::make_limit_order(203, 1003, "APL", 200, 103.00, Side::Buy, TimeInForce::FOK);
            mtche.match_order(buyFOK); // will throw if not fully matchable
            std::cout << "\nFOK filled fully. Remaining: " << buyFOK.remaining << "\n";
        } catch (const std::exception& e) {
            std::cout << "\nFOK rejected as expected: " << e.what() << "\n";
        }

        // Non-crossing resting bid: BUY 20 @ 95 (will rest on the bid side)
        Order restingBid = Order::make_limit_order(204, 1004, "AAPL", 20, 95.00, Side::Buy, TimeInForce::GTC);
        mtche.match_order(restingBid);
        std::cout << "\nPlaced resting BUY 20 @ 95 (GTC). Remaining: " << restingBid.remaining << "\n";
        registry.print_markets();

        // Show a market order object creation (not processed since not implemented)
        Order mktSell = Order::make_market_order(205, 1005, "AAPL", 10, Side::Sell, TimeInForce::Day);
        std::cout << "\nCreated (but NOT processed) Market Sell 10 on AAPL:\n" << to_string(mktSell) << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
