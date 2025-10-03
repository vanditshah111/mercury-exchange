#pragma once
#include "MarketEvent.hpp"
#include <vector>

namespace MercEx {
    class IMarketDataListener {
    public:
        virtual ~IMarketDataListener() = default;
        virtual void on_market_events(const std::vector<MarketEvent>& events) = 0;
    };
}