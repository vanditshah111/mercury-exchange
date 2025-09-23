#pragma once
#include <optional>
#include "Order.hpp"
#include "Trade.hpp"

namespace MercEx {

    enum class MarketEventType {
        OrderAccepted,
        OrderPartiallyFilled,
        OrderFilled,
        OrderCanceled,
        TradeOccurred,
        StopOrderTriggered
    };

    struct MarketEvent {
        MarketEventType type;
        OrderID order_id;
        std::optional<Trade> trade;                 
        std::optional<OrderStatus> new_status;   
    };

}
