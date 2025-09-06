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
            std::optional<std::list<Order>::iterator> it=market->process_order(order);

            if(it!=std::nullopt)
            {
                OrderLocator ol;
                ol.it=*it;
                ol.price=*order.price;
                ol.side=order.side;
                ol.symbol=order.symbol;
                order_index[order.id]=ol;
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
            market->get_buybook().cancel_order(id, loc.price, loc.it);
        }
        else
        {
            market->get_sellbook().cancel_order(id, loc.price, loc.it);
        }
        return true;
    }

} // namespace MercEx
