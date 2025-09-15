// buybook.hpp
#pragma once
#include <cstdint>
#include <map>
#include <list>
#include <optional>
#include <functional>
#include "order.hpp"

namespace MercEx {

    class BuyBook {
    public:
        BuyBook() = default;

        std::list<Order*>::iterator add_order(Order& order);
        bool cancel_order(OrderID id, Price price, std::list<Order*>::iterator it);

        std::map<Price, std::list<Order*>, std::greater<>>& get_orders() { return orders_; }
        const std::map<Price, std::list<Order*>, std::greater<>>& get_orders() const { return orders_; }

        std::optional<std::reference_wrapper<Order>> get_best_order();
        std::optional<Price> get_best_bid() const;

        bool empty() const;
        std::size_t size() const;

    private:
        std::map<Price, std::list<Order*>, std::greater<>> orders_;
    };

} // namespace MercEx
