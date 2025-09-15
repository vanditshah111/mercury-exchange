// buybook.cpp
#include "../include/SellBook.hpp"
#include <algorithm>
#include <stdexcept>

namespace MercEx {

    std::list<Order*>::iterator SellBook::add_order(Order& order) {
        auto& q = orders_[order.price.value()];
        q.push_back(&order);
        order.book_it = std::prev(q.end());
        return order.book_it;
    }

    bool SellBook::cancel_order(OrderID id, Price price, std::list<Order*>::iterator it) {
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

    std::optional<std::reference_wrapper<Order>> SellBook::get_best_order() {
        if (orders_.empty()) return std::nullopt;
        return *(orders_.begin()->second.front());
    }

    std::optional<Price> SellBook::get_best_ask() const {
        if (orders_.empty()) return std::nullopt;
        return orders_.begin()->second.front()->price;
    }

    bool SellBook::empty() const {
        return orders_.empty();
    }

    std::size_t SellBook::size() const {
        std::size_t count = 0;
        for (const auto& [price, order_list] : orders_)
            count += order_list.size();
        return count;
    }

} // namespace MercEx
