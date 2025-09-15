#include "../include/MatchingEngine.hpp"
#include <stdexcept>

namespace MercEx
{

    MatchingEngine::MatchingEngine(MarketRegistry &registry)
        : registry(registry) {}

    OrderID MatchingEngine::generate_order_id() {
        return next_id++;
    }

    OrderID MatchingEngine::submit_order(ClientID client_id,
                                         const std::string& symbol,
                                         Quantity quantity,
                                         Side side,
                                         std::optional<Price> price,
                                         OrderType type,
                                         TimeInForce tif)
    {
        switch (type) {
            case OrderType::Limit:
                if (!price) throw std::invalid_argument("Limit order requires price");
                return submit_limit_order(client_id, symbol, quantity, *price, side, tif);

            case OrderType::Market:
                if (price) throw std::invalid_argument("Market order must not have price");
                return submit_market_order(client_id, symbol, quantity, side, tif);

            case OrderType::Stop:
                throw std::logic_error("Stop orders not implemented yet");
        }
        throw std::invalid_argument("Unknown order type");
    }

    OrderID MatchingEngine::submit_limit_order(ClientID client_id,
                                               const std::string& symbol,
                                               Quantity quantity,
                                               Price price,
                                               Side side,
                                               TimeInForce tif)
    {
        OrderID id = generate_order_id();
        auto order = Order::make_limit_order(id, client_id, symbol, quantity, price, side, tif);

        Order* raw = order.get();
        all_orders.emplace(id, std::move(order));

        Market* market = registry.get_market(symbol);
        if (!market) throw std::runtime_error("Unknown market: " + symbol);

        market->process_order(*raw);
        return id;
    }

    OrderID MatchingEngine::submit_market_order(ClientID client_id,
                                                const std::string& symbol,
                                                Quantity quantity,
                                                Side side,
                                                TimeInForce tif)
    {
        OrderID id = generate_order_id();
        auto order = Order::make_market_order(id, client_id, symbol, quantity, side, tif);

        Order* raw = order.get();
        all_orders.emplace(id, std::move(order));

        Market* market = registry.get_market(symbol);
        if (!market) throw std::runtime_error("Unknown market: " + symbol);

        market->process_order(*raw);
        return id;
    }

    bool MatchingEngine::cancel_order(OrderID id) {
        auto it = all_orders.find(id);
        if (it == all_orders.end()) return false;

        Order* order = it->second.get();
        if (order->status == OrderStatus::Canceled ||
            order->status == OrderStatus::Filled) {
            return false;
        }

        Market* market = registry.get_market(order->symbol);
        if (!market) return false;

        bool success = market->cancel_order(order);
        if (success) {
            order->status = OrderStatus::Canceled;
        }
        return success;
    }

    const Order *MatchingEngine::get_order(OrderID id) const
    {
        auto it = all_orders.find(id);
        if (it == all_orders.end()) return nullptr;

        Order* order = it->second.get();
        
        return order;
    }

} // namespace MercEx
