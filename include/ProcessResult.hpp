#pragma once
#include <vector>
#include <optional>
#include "Order.hpp"
#include "Trade.hpp"
#include <list>

namespace MercEx
{
    struct ProcessResult
    {
        std::optional<std::list<Order*>::iterator> resting_order;
        std::vector<Trade> trades;
        std::vector<OrderID> removed_orders;
    };
}