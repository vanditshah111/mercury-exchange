#include "../include/Order.hpp"
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <cmath>

namespace MercEx
{

    std::string to_string(Side side)
    {
        switch (side)
        {
        case Side::Buy:
            return "Buy";
        case Side::Sell:
            return "Sell";
        default:
            throw std::invalid_argument("Invalid Side value");
        }
    }

    std::string to_string(OrderType type)
    {
        switch (type)
        {
        case OrderType::Limit:
            return "Limit";
        case OrderType::Market:
            return "Market";
        case OrderType::Stop:
            return "Stop";
        default:
            throw std::invalid_argument("Invalid OrderType value");
        }
    }

    std::string to_string(TimeInForce tif)
    {
        switch (tif)
        {
        case TimeInForce::Day:
            return "Day";
        case TimeInForce::IOC:
            return "IOC";
        case TimeInForce::FOK:
            return "FOK";
        case TimeInForce::GTC:
            return "GTC";
        default:
            throw std::invalid_argument("Invalid TimeInForce value");
        }
    }

    std::string to_string(OrderStatus status)
    {
        switch (status)
        {
        case OrderStatus::New:
            return "New";
        case OrderStatus::PartiallyFilled:
            return "PartiallyFilled";
        case OrderStatus::Filled:
            return "Filled";
        case OrderStatus::Canceled:
            return "Canceled";
        case OrderStatus::Expired:
            return "Expired";
        default:
            throw std::invalid_argument("Invalid OrderStatus value");
        }
    }

    std::string to_string(const Order &order)
    {
        std::ostringstream oss;
        oss << "Order ID: " << order.id << "\n"
            << "Client ID: " << order.client_id << "\n"
            << "Symbol: " << order.symbol << "\n"
            << "Timestamp: " << std::chrono::duration_cast<std::chrono::milliseconds>(order.timestamp.time_since_epoch()).count() << " ms\n"
            << "Quantity: " << order.quantity << "\n"
            << "Remaining: " << order.remaining << "\n"
            << "Price: " << (order.price ? std::to_string(*order.price) : "N/A") << "\n"
            << "Side: " << to_string(order.side) << "\n"
            << "Type: " << to_string(order.type) << "\n"
            << "Time In Force: " << to_string(order.tif) << "\n";
        return oss.str();
    }

    Side to_side(const std::string &str)
    {
        if (str == "Buy")
            return Side::Buy;
        if (str == "Sell")
            return Side::Sell;
        throw std::invalid_argument("Invalid Side string");
    }

    OrderType to_order_type(const std::string &str)
    {
        if (str == "Limit")
            return OrderType::Limit;
        if (str == "Market")
            return OrderType::Market;
        if (str == "Stop")
            return OrderType::Stop;
        throw std::invalid_argument("Invalid OrderType string");
    }

    TimeInForce to_time_in_force(const std::string &str)
    {
        if (str == "Day")
            return TimeInForce::Day;
        if (str == "IOC")
            return TimeInForce::IOC;
        if (str == "FOK")
            return TimeInForce::FOK;
        if (str == "GTC")
            return TimeInForce::GTC;
        throw std::invalid_argument("Invalid TimeInForce string");
    }

    Order Order::make_limit_order(OrderID id, ClientID client_id, const std::string &symbol,
                                  Quantity quantity, Price price, Side side, TimeInForce tif)
    {
        Order o;
        o.id = id;
        o.client_id = client_id;
        o.symbol = symbol;
        o.timestamp = Clock::now();
        o.quantity = quantity;
        o.remaining = quantity;
        o.price = price;
        o.side = side;
        o.type = OrderType::Limit;
        o.tif = tif;
        o.status = OrderStatus::New;
        o.validate();
        return o;
    }

    Order Order::make_market_order(OrderID id, ClientID client_id, const std::string &symbol,
                                   Quantity quantity, Side side, TimeInForce tif)
    {
        Order o;
        o.id = id;
        o.client_id = client_id;
        o.symbol = symbol;
        o.timestamp = Clock::now();
        o.quantity = quantity;
        o.remaining = quantity;
        o.price = std::nullopt;
        o.side = side;
        o.type = OrderType::Market;
        o.tif = tif;
        o.status = OrderStatus::New;
        o.validate();
        return o;
    }

    void Order::validate() const
    {
        if (symbol.empty())
            throw std::invalid_argument("Order.symbol empty");
        if (quantity <= 0)
            throw std::invalid_argument("Order.quantity must be > 0");
        if (remaining < 0 || remaining > quantity)
            throw std::invalid_argument("Order.remaining out of range");
        if (type == OrderType::Limit && !price.has_value())
            throw std::invalid_argument("Limit order must have price");
        if (type == OrderType::Market && price.has_value())
            throw std::invalid_argument("Market order must not have price");
        if (price.has_value() && *price <= 0)
            throw std::invalid_argument("Order.price must be > 0");
    }

}
