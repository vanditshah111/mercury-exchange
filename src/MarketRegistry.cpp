#include "../include/MarketRegistry.hpp"
#include <iomanip>

namespace MercEx {

Market& MarketRegistry::create_market(const std::string& symbol, double price_tick) {
    if (markets.find(symbol) != markets.end()) {
        throw std::invalid_argument("Market with symbol already exists");
    }
    markets.emplace(symbol, Market(symbol, price_tick));
    return markets.at(symbol);
}

Market* MarketRegistry::get_market(const std::string& symbol) {
    auto it = markets.find(symbol);
    if (it != markets.end()) {
        return &it->second;
    }
    return nullptr;
}

bool MarketRegistry::remove_market(const std::string& symbol) {
    return markets.erase(symbol) > 0;
}

void MarketRegistry::print_markets() const {
    std::cout << "Market Symbol\tLast Price\tBid Price\tAsk Price\n";
    for (const auto& pair : markets) {
        const auto& market = pair.second;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::setw(8) << pair.first
          << std::setw(12) << (market.get_last_price() ? std::to_string(*market.get_last_price()) : "N/A")
          << std::setw(12) << (market.get_bid_price()  ? std::to_string(*market.get_bid_price())  : "N/A")
          << std::setw(12) << (market.get_ask_price()  ? std::to_string(*market.get_ask_price())  : "N/A")
          << "\n";

    }
}

} // namespace MercEx
