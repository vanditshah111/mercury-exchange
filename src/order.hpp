#pragma once
#include <cstdint>
#include <string>
#include <stdexcept>
#include <optional>
#include <chrono>
#include <limits>
#include <sstream>

namespace MercEx{
    using OrderID = std::uint64_t;
    using ClientID = std::uint32_t;
    using Quantity = std::int32_t;
    using Price = double;
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    enum class Side : std::uint8_t { Buy, Sell };
    enum class OrderType : std::uint8_t { Limit, Market, Stop };
    enum class TimeInForce : std::uint8_t { Day, IOC, FOK, GTC };

    inline std::string to_string(Side side) {
        switch (side) {
            case Side::Buy: return "Buy";
            case Side::Sell: return "Sell";
            default: throw std::invalid_argument("Invalid Side value");
        }
    }

    inline std::string to_string(OrderType type) {
        switch (type) {
            case OrderType::Limit: return "Limit";
            case OrderType::Market: return "Market";
            case OrderType::Stop: return "Stop";
            default: throw std::invalid_argument("Invalid OrderType value");
        }
    }

    inline std::string to_string(TimeInForce tif) {
        switch (tif) {
            case TimeInForce::Day: return "Day";
            case TimeInForce::IOC: return "IOC";
            case TimeInForce::FOK: return "FOK";
            case TimeInForce::GTC: return "GTC";
            default: throw std::invalid_argument("Invalid TimeInForce value");
        }
    }

    inline std::string to_string(const Order& order) {
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

    inline Side to_side(const std::string& str) {
        if (str == "Buy") return Side::Buy;
        if (str == "Sell") return Side::Sell;
        throw std::invalid_argument("Invalid Side string");
    }

    inline OrderType to_order_type(const std::string& str) {
        if (str == "Limit") return OrderType::Limit;
        if (str == "Market") return OrderType::Market;
        if (str == "Stop") return OrderType::Stop;
        throw std::invalid_argument("Invalid OrderType string");
    }

    inline TimeInForce to_time_in_force(const std::string& str) {
        if (str == "Day") return TimeInForce::Day;
        if (str == "IOC") return TimeInForce::IOC;
        if (str == "FOK") return TimeInForce::FOK;
        if (str == "GTC") return TimeInForce::GTC;
        throw std::invalid_argument("Invalid TimeInForce string");
    }

    struct Order {
        OrderID id;
        ClientID client_id;
        std::string symbol;
        TimePoint timestamp;
        Quantity quantity;
        Quantity remaining;
        std::optional<Price> price;
        Side side;
        OrderType type;
        TimeInForce tif;

        static Order make_limit_order(OrderID id, ClientID client_id, const std::string& symbol, Quantity quantity, Price price, Side side, TimeInForce tif) {
            if (quantity <= 0) throw std::invalid_argument("Quantity must be positive");
            if (price <= 0.0) throw std::invalid_argument("Price must be positive for limit orders");
            return Order{id, client_id, symbol, Clock::now(), quantity, quantity, price, side, OrderType::Limit, tif};
        }

        static Order make_market_order(OrderID id, ClientID client_id, const std::string& symbol, Quantity quantity, Side side, TimeInForce tif) {
            if (quantity <= 0) throw std::invalid_argument("Quantity must be positive");
            return Order{id, client_id, symbol, Clock::now(), quantity, quantity, std::nullopt, side, OrderType::Market, tif};
        }
    };
};
