#pragma once
#include <string>
#include <vector>
#include <optional>
#include "BuyBook.hpp"
#include "SellBook.hpp"

namespace MercEx
{
    class Market
    {
    public:
        Market(const std::string &symbol, double price_tick);

        bool is_valid_price(double price) const;
        bool validate_fulfillment(const Order &order, Side side);
        std::optional<std::list<Order>::iterator> process_order(Order &order);
        std::optional<std::list<Order>::iterator> process_buy_order(Order &order);
        std::optional<std::list<Order>::iterator> process_sell_order(Order &order);

        void delete_order(Order &order);

        const std::string &get_symbol() const;
        double get_price_tick() const;
        bool active() const;
        void activate();
        void deactivate();

        BuyBook &get_buybook();
        SellBook &get_sellbook();

        std::optional<double> get_last_price() const;
        void update_last_price(double price);

        std::optional<Price> get_bid_price() const;
        std::optional<Price> get_ask_price() const;

    private:
        std::string symbol;
        double price_tick;
        bool is_active;
        BuyBook buybook;
        SellBook sellbook;
        std::optional<double> last_price;
    };
}
