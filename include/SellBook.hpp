#pragma once
#include <cstdint>
#include <map>
#include <list>
#include <optional>
#include <functional>
#include "order.hpp"

namespace MercEx {

class SellBook {
public:
    SellBook() = default;

    std::list<Order>::iterator add_order(const Order& order);
    bool cancel_order(OrderID id, double price, std::list<Order>::iterator it);
    std::map<double, std::list<Order>, std::less<>>& get_orders() { return orders_; }
    std::optional<std::reference_wrapper<Order>> get_best_order();
    std::optional<Price> get_best_ask() const;
    bool empty() const;
    std::size_t size() const;

private:
    std::map<double, std::list<Order>, std::less<>> orders_;
};

} // namespace MercEx
