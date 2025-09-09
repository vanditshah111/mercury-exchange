#include "../include/MatchingEngine.hpp"
#include <stdexcept>

namespace MercEx
{

    MatchingEngine::MatchingEngine(MarketRegistry &registry)
        : registry(registry) {}

    void MatchingEngine::match_order(Order &order)
    {
        auto *market = registry.get_market(order.symbol);
        if (market)
        {
            ProcessResult result = market->process_order(order);
            std::optional<std::list<Order>::iterator> it = result.resting_order;

            if (it != std::nullopt)
            {
                OrderLocator ol;
                ol.it = *it;
                ol.price = *order.price;
                ol.side = order.side;
                ol.symbol = order.symbol;
                order_index[order.id] = ol;
            }
        }
        else
        {
            throw std::invalid_argument("Market not found for order");
        }
    }

    bool MatchingEngine::cancel_order(OrderID id)
    {
        auto it = order_index.find(id);
        if (it == order_index.end())
        {
            return false;
        }

        auto loc = it->second;
        Market *market = registry.get_market(loc.symbol);
        if (!market)
            return false;

        if (loc.side == Side::Buy)
        {
            if(market->get_buybook().cancel_order(id, loc.price, loc.it))
            {
                loc.it->status = OrderStatus::Canceled;
                return true;
            }
        }
        else
        {
            if(market->get_sellbook().cancel_order(id, loc.price, loc.it))
            {
                loc.it->status = OrderStatus::Canceled;
                return true;
            }
        }
        return false;
    }

} // namespace MercEx
