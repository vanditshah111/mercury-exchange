#include "Market.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>

namespace MercEx
{

    Market::Market(const std::string &symbol, double price_tick)
        : symbol(symbol), price_tick(price_tick), is_active(true), buybook(), sellbook(), last_price(std::nullopt) {}

    bool Market::is_valid_price(double price) const
    {
        return std::fmod(price * 100, price_tick * 100) == 0.0;
    }

    bool Market::validate_fulfillment(const Order &order, Side side)
    {
        int total_matched_quantity = 0;
        if (side == Side::Buy)
        {
            for (const auto &[price, orders] : sellbook.get_orders())
            {
                if (order.price >= price)
                {
                    for (const auto &o : orders)
                        total_matched_quantity += o.quantity;
                }
                else
                    break;
            }
        }
        else if (side == Side::Sell)
        {
            for (const auto &[price, orders] : buybook.get_orders())
            {
                if (order.price <= price)
                {
                    for (const auto &o : orders)
                        total_matched_quantity += o.quantity;
                }
                else
                    break;
            }
        }
        return total_matched_quantity >= order.quantity;
    }

    void Market::delete_order(Order &order)
    {
    }
    std::optional<std::list<Order>::iterator> Market::process_order(Order &order)
    {
        if (!is_active)
            throw std::runtime_error("Market is inactive");

        if (order.type == OrderType::Limit)
        {
            if (!order.price.has_value() || !is_valid_price(order.price.value()))
                throw std::invalid_argument("Invalid price for limit order");

            if (order.side == Side::Buy)
                return process_buy_order(order);
            else if (order.side == Side::Sell)
                return process_sell_order(order);
            else
                throw std::invalid_argument("Invalid order side");
        }
        else if (order.type == OrderType::Market)
        {
            
        }
        else
        {
            throw std::invalid_argument("Unsupported order type");
        }
    }

    std::optional<std::list<Order>::iterator> Market::process_buy_order(Order &order)
    {
        if (!validate_fulfillment(order, Side::Buy) && order.tif == TimeInForce::FOK)
            throw std::runtime_error("FOK order cannot be fulfilled");

        for (auto price_it = sellbook.get_orders().begin(); price_it != sellbook.get_orders().end();)
        {
            auto &orders = price_it->second;

            for (auto it = orders.begin(); it != orders.end();)
            {
                if (order.remaining <= 0)
                    break;

                int trade_quantity = std::min(order.remaining, it->remaining);
                order.remaining -= trade_quantity;
                it->remaining -= trade_quantity;

                if (it->remaining == 0)
                {
                    it = orders.erase(it);
                }
                else
                {
                    ++it;
                }

                update_last_price(price_it->first);
            }

            if (orders.empty())
            {
                price_it = sellbook.get_orders().erase(price_it);
            }
            else
            {
                ++price_it;
            }

            if (order.remaining <= 0)
                break;
        }

        if (order.tif != TimeInForce::IOC && order.remaining > 0)
            return buybook.add_order(order);

        return std::nullopt;
    }

    std::optional<std::list<Order>::iterator> Market::process_sell_order(Order &order)
    {
        if (!validate_fulfillment(order, Side::Sell) && order.tif == TimeInForce::FOK)
            throw std::runtime_error("FOK order cannot be fulfilled");

        for (auto price_it = buybook.get_orders().begin(); price_it != buybook.get_orders().end();)
        {
            auto &orders = price_it->second;

            for (auto it = orders.begin(); it != orders.end();)
            {
                if (order.remaining <= 0)
                    break;

                int trade_quantity = std::min(order.remaining, it->remaining);
                order.remaining -= trade_quantity;
                it->remaining -= trade_quantity;

                if (it->remaining == 0)
                {
                    it = orders.erase(it);
                }
                else
                {
                    ++it;
                }

                update_last_price(price_it->first);
            }

            if (orders.empty())
            {
                price_it = buybook.get_orders().erase(price_it);
            }
            else
            {
                ++price_it;
            }

            if (order.remaining <= 0)
                break;
        }

        if (order.tif != TimeInForce::IOC && order.remaining > 0)
            return sellbook.add_order(order);
        
        return std::nullopt;
    }

    const std::string &Market::get_symbol() const { return symbol; }
    double Market::get_price_tick() const { return price_tick; }
    bool Market::active() const { return is_active; }
    void Market::activate() { is_active = true; }
    void Market::deactivate() { is_active = false; }

    BuyBook &Market::get_buybook() { return buybook; }
    SellBook &Market::get_sellbook() { return sellbook; }

    void Market::print_order_books() const
    {
        std::cout << "Buy Orders:\n";
        for (const auto &[price, orders] : buybook.get_orders())
        {
            for (const auto &order : orders)
            {
                std::cout << "Price: " << price << ", OrderID: " << order.id << ", Quantity: " << order.quantity << ", Remaining: " << order.remaining << "\n";
            }
        }

        std::cout << "Sell Orders:\n";
        for (const auto &[price, orders] : sellbook.get_orders())
        {
            for (const auto &order : orders)
            {
                std::cout << "Price: " << price << ", OrderID: " << order.id << ", Quantity: " << order.quantity << ", Remaining: " << order.remaining << "\n";
            }
        }
    }
    std::optional<double> Market::get_last_price() const { return last_price; }
    void Market::update_last_price(double price) { last_price = price; }

    std::optional<Price> Market::get_bid_price() const { return buybook.get_best_bid(); }
    std::optional<Price> Market::get_ask_price() const { return sellbook.get_best_ask(); }

} // namespace MercEx
