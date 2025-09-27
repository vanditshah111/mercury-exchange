#pragma once
#include <string>
#include <vector>
#include <optional>
#include "BuyBook.hpp"
#include "SellBook.hpp"
#include "Trade.hpp"
#include "ProcessResult.hpp"
#include "MarketEvent.hpp"

namespace MercEx
{

    class Market
    {
    public:
        Market(const std::string &symbol, double price_tick);

        bool is_valid_price(double price) const;
        bool validate_fulfillment(const Order &order, Side side);
        std::vector<MarketEvent> process_order(Order &order);
        std::vector<MarketEvent> process_limit_buy_order(Order &order);
        std::vector<MarketEvent> process_limit_sell_order(Order &order);
        std::vector<MarketEvent> process_market_buy_order(Order &order);
        std::vector<MarketEvent> process_market_sell_order(Order &order);

        bool cancel_order(Order *order);

        const std::string &get_symbol() const;
        double get_price_tick() const;
        bool active() const;
        void activate();
        void deactivate();

        BuyBook &get_buybook();
        SellBook &get_sellbook();

        void print_order_books() const;

        std::optional<double> get_last_price() const;
        void update_last_price(double price);

        std::optional<Price> get_bid_price() const;
        std::optional<Price> get_ask_price() const;

        MarketID get_market_id() const;

    private:
        std::string symbol;
        MarketID market_id;
        double price_tick;
        bool is_active;
        BuyBook buybook;
        SellBook sellbook;
        std::optional<double> last_price;
    };
}
