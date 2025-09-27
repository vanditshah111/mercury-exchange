#include "MatchingEngine.hpp"
#include <stdexcept>
#include <chrono>

namespace MercEx {

MatchingEngine::MatchingEngine(MarketRegistry& registry)
    : registry_(registry) {}

OrderID MatchingEngine::submit_order(ClientID client_id,
                                     const std::string& symbol,
                                     Quantity quantity,
                                     Side side,
                                     std::optional<Price> price,
                                     OrderType type,
                                     TimeInForce tif,
                                     std::optional<Price> stop_price)
{
    auto* processor = registry_.get_market_processor(symbol);
    if (!processor) {
        throw std::invalid_argument("Unknown market: " + symbol);
    }

    MarketID market_id = processor->get_market_id();
    OrderID id = generate_order_id(market_id);

    MarketEvent ev;
    ev.type = MarketEventType::AddOrder;
    ev.order_id = id;
    ev.client_id = client_id;
    ev.symbol = symbol;
    ev.quantity = quantity;
    ev.side = side;
    ev.price = price;
    ev.stop_price = stop_price;
    ev.order_type = type;
    ev.tif = tif;
    ev.timestamp = std::chrono::steady_clock::now();

    processor->submit_event(ev);

    return id;
}

bool MatchingEngine::cancel_order(OrderID id, const std::string& symbol) {
    auto* processor = registry_.get_market_processor(symbol);
    if (!processor) return false;

    MarketEvent ev;
    ev.type = MarketEventType::CancelOrder;
    ev.order_id = id;
    ev.timestamp = std::chrono::steady_clock::now();

    processor->submit_event(ev);
    return true;
}

OrderID MatchingEngine::generate_order_id(MarketID market_id) {
    uint64_t counter = market_counters_[market_id].fetch_add(1, std::memory_order_relaxed);
    return (static_cast<OrderID>(market_id) << 48) | (counter & 0x0000FFFFFFFFFFFFULL);
}

} // namespace MercEx
