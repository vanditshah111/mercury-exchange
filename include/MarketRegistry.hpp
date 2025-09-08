#pragma once
#include "Market.hpp"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace MercEx
{

    class MarketRegistry
    {
    public:
        Market &create_market(const std::string &symbol, double price_tick);

        Market *get_market(const std::string &symbol);

        bool remove_market(const std::string &symbol);

        void print_markets() const;

    private:
        std::unordered_map<std::string, Market> markets;
    };
}