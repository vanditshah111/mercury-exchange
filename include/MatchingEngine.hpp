#pragma once
#include "MarketRegistry.hpp"
#include "MarketEvent.hpp"
#include <atomic>
#include <unordered_map>
#include <memory>

namespace MercEx
{

    class MatchingEngine
    {
    public:
        explicit MatchingEngine(MarketRegistry &registry);

        OrderID submit_order(ClientID client_id,
                             const std::string &symbol,
                             Quantity quantity,
                             Side side,
                             std::optional<Price> price,
                             OrderType type,
                             TimeInForce tif,
                             std::optional<Price> stop_price = std::nullopt);

        bool cancel_order(OrderID id, const std::string &symbol);

        //const Order *get_order(OrderID id, const std::string &symbol) const;

    private:
        MarketRegistry &registry_;

        std::unordered_map<uint16_t, std::atomic<uint64_t>> market_counters_;

        OrderID generate_order_id(uint16_t market_id);
    };

} // namespace MercEx
