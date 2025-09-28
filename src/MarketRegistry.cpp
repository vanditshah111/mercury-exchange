#include "MarketRegistry.hpp"
#include <iostream>
#include <iomanip>

namespace MercEx
{

    MarketProcessor &MarketRegistry::create_market(const std::string &symbol, double price_tick, uint16_t market_id)
    {
        if (processors_.find(symbol) != processors_.end())
        {
            throw std::invalid_argument("Market with symbol already exists");
        }

        auto market = std::make_unique<Market>(symbol, price_tick);
        auto processor = std::make_unique<MarketProcessor>(std::move(market));
        processors_[symbol] = std::move(processor);
        processors_[symbol]->start();
        return *processors_[symbol];
    }

    MarketProcessor *MarketRegistry::get_market_processor(const std::string &symbol)
    {
        auto it = processors_.find(symbol);
        return it != processors_.end() ? it->second.get() : nullptr;
    }

    bool MarketRegistry::remove_market(const std::string &symbol)
    {
        auto it = processors_.find(symbol);
        if (it != processors_.end())
        {
            it->second->stop(); // stop the processor thread
            processors_.erase(it);
            return true;
        }
        return false;
    }

    void MarketRegistry::print_markets() const
    {
        std::cout << "Market Symbol\tLast Price\tBid Price\tAsk Price\n";
        for (const auto &pair : processors_)
        {
            const auto &proc = pair.second;
            const auto &market = proc->get_market(); // assuming Market& getter in MarketProcessor
            std::cout << std::fixed << std::setprecision(2);
            std::cout << std::setw(8) << pair.first
                      << std::setw(12) << (market.get_last_price() ? std::to_string(*market.get_last_price()) : "N/A")
                      << std::setw(12) << (market.get_bid_price() ? std::to_string(*market.get_bid_price()) : "N/A")
                      << std::setw(12) << (market.get_ask_price() ? std::to_string(*market.get_ask_price()) : "N/A")
                      << "\n";
        }
    }

} // namespace MercEx
