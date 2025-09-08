#include "../include/SellBook.hpp"
#include <algorithm>
#include <stdexcept>

namespace MercEx
{

    std::list<Order>::iterator SellBook::add_order(const Order &order)
    {
        if (order.side != Side::Sell)
            throw std::invalid_argument("Order side mismatch for SellBook");
        if (order.type != OrderType::Limit || !order.price.has_value())
            throw std::invalid_argument("Only limit orders with price can be added to SellBook");
        orders_[order.price.value()].push_back(order);
        return std::prev(orders_[order.price.value()].end());
    }

    bool SellBook::cancel_order(OrderID id, double price, std::list<Order>::iterator it)
    {
        auto price_it = orders_.find(price);
        if (price_it != orders_.end())
        {
            price_it->second.erase(it);
            if (price_it->second.empty())
            {
                orders_.erase(price_it);
            }
            return true;
        }
        return false;
    }

    std::optional<std::reference_wrapper<Order>> SellBook::get_best_order()
    {
        if (orders_.empty())
            return std::nullopt;
        return orders_.begin()->second.front();
    }

    std::optional<Price> SellBook::get_best_ask() const
    {
        if (orders_.empty())
            return std::nullopt;
        return orders_.begin()->second.front().price;
    }

    bool SellBook::empty() const
    {
        return orders_.empty();
    }

    std::size_t SellBook::size() const
    {
        std::size_t count = 0;
        for (const auto &[price, order_list] : orders_)
            count += order_list.size();
        return count;
    }

} // namespace MercEx
