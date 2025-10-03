#include "MatchingEngine.hpp"
#include "MarketRegistry.hpp"
#include "MarketDataPublisher.hpp" // <-- Add
#include "IMarketDataListener.hpp" // <-- Add
#include <iostream>
// ...

using namespace MercEx;

// Simple console logger for demonstration
class ConsoleLogger : public IMarketDataListener {
public:
    void on_market_events(const std::vector<MarketEvent>& events) override {
        for (const auto& event : events) {
            if (event.type == MarketEventType::Trade) {
                std::cout << "[TRADE FEED] " << *event.executed_qty << " @ " << *event.executed_price << std::endl;
            }
        }
    }
};


int main() {
    // 1. Create the single publisher for the whole exchange
    MarketDataPublisher publisher;

    // 2. Create the registry, passing the publisher to it
    MarketRegistry registry(publisher);

    // 3. Create a listener and subscribe it to the publisher
    ConsoleLogger logger;
    publisher.subscribe(&logger);

    // 4. The rest of your application logic
    double tick = 0.5;
    registry.create_market("AAPL", tick, 1);
    registry.create_market("GOOG", tick, 2);

    MatchingEngine engine(registry);

    // Submit a trade to see the feed in action
    engine.submit_order(1, "AAPL", 100, Side::Buy, 150.0, OrderType::Limit, TimeInForce::GTC);
    engine.submit_order(2, "AAPL", 50, Side::Sell, 150.0, OrderType::Limit, TimeInForce::GTC);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    return 0;
}