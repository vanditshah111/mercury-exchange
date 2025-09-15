#pragma once
#include "MarketRegistry.hpp"
#include "Order.hpp"
#include "Market.hpp"

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
                             TimeInForce tif);

        bool cancel_order(OrderID id);

        const Order *get_order(OrderID id) const;

    private:
        MarketRegistry &registry;
        std::unordered_map<OrderID, std::unique_ptr<Order>> all_orders;

        OrderID next_id = 1;

        OrderID generate_order_id();

        OrderID submit_limit_order(
            ClientID client_id,
            const std::string &symbol,
            Quantity qty,
            Price price,
            Side side,
            TimeInForce tif);

        OrderID submit_market_order(
            ClientID client_id,
            const std::string &symbol,
            Quantity qty,
            Side side,
            TimeInForce tif);
    };

} // namespace MercEx
