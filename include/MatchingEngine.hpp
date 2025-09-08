#pragma once
#include "MarketRegistry.hpp"
#include "Order.hpp"

namespace MercEx
{

    class MatchingEngine
    {
    public:
        struct OrderLocator
        {
            std::string symbol;
            Side side;
            double price;
            std::list<Order>::iterator it;
        };

        MatchingEngine(MarketRegistry &registry);

        void match_order(Order &order);

        bool cancel_order(OrderID id);

    private:
        MarketRegistry &registry;
        std::unordered_map<OrderID, OrderLocator> order_index;
    };

} // namespace MercEx
