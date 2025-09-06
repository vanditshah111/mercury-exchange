#include "../include/BuyBook.hpp"
#include <algorithm>
#include <stdexcept>

namespace MercEx {

std::list<Order>::iterator BuyBook::add_order(const Order& order) {
    if (order.side != Side::Buy)
        throw std::invalid_argument("Order side mismatch for BuyBook");
    if (order.type != OrderType::Limit || !order.price.has_value())
        throw std::invalid_argument("Only limit orders with price can be added to BuyBook");
    orders_[order.price.value()].push_back(order);
    return std::prev(orders_[order.price.value()].end());
}

bool BuyBook::cancel_order(OrderID id, double price, std::list<Order>::iterator it) {
        auto price_it = orders_.find(price);
        if (price_it != orders_.end()) {
            price_it->second.erase(it);
            if (price_it->second.empty()) {
                orders_.erase(price_it);
            }
            return true;
        }
        return false;
}

std::optional<std::reference_wrapper<Order>> BuyBook::get_best_order() {
    if (orders_.empty()) return std::nullopt;
    return orders_.begin()->second.front();
}

std::optional<Price> BuyBook::get_best_bid() const {
    if (orders_.empty()) return std::nullopt;
    return orders_.begin()->second.front().price;
}

bool BuyBook::empty() const {
    return orders_.empty();
}

std::size_t BuyBook::size() const {
    std::size_t count = 0;
    for (const auto& [price, order_list] : orders_)
        count += order_list.size();
    return count;
}

} // namespace MercEx
