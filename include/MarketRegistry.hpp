#pragma once
#include "MarketProcessor.hpp"
#include <unordered_map>
#include <memory>
#include <string>

namespace MercEx {

class MarketRegistry {
public:
    MarketProcessor& create_market(const std::string& symbol, double price_tick, uint16_t market_id);
    MarketProcessor* get_market_processor(const std::string& symbol);
    bool remove_market(const std::string& symbol);
    void print_markets() const;

private:
    std::unordered_map<std::string, std::unique_ptr<MarketProcessor>> processors_;
};

} // namespace MercEx
