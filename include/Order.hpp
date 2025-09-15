#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <chrono>
#include <memory>
#include <list>

namespace MercEx
{

    using OrderID = std::uint64_t;
    using ClientID = std::uint32_t;
    using Quantity = std::int32_t;
    using Price = double;
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    enum class Side : std::uint8_t
    {
        Buy,
        Sell
    };
    enum class OrderType : std::uint8_t
    {
        Limit,
        Market,
        Stop
    };
    enum class TimeInForce : std::uint8_t
    {
        Day,
        IOC,
        FOK,
        GTC
    };
    enum class OrderStatus : std::uint8_t
    {
        New,
        PartiallyFilled,
        Filled,
        Canceled,
        Expired
    };

    struct Order;

    std::string to_string(Side side);
    std::string to_string(OrderType type);
    std::string to_string(TimeInForce tif);
    std::string to_string(OrderStatus status);
    std::string to_string(const Order &order);

    Side to_side(const std::string &str);
    OrderType to_order_type(const std::string &str);
    TimeInForce to_time_in_force(const std::string &str);

    struct Order
    {
        OrderID id;
        ClientID client_id;
        std::string symbol;
        TimePoint timestamp;
        Quantity quantity;
        Quantity remaining;
        std::optional<Price> price;
        Side side;
        OrderStatus status;
        OrderType type;
        TimeInForce tif;
        std::list<Order*>::iterator book_it = {};
        
        static std::unique_ptr<Order> make_limit_order(OrderID id, ClientID client_id, const std::string &symbol,
                                      Quantity quantity, Price price, Side side, TimeInForce tif);
        static std::unique_ptr<Order> make_market_order(OrderID id, ClientID client_id, const std::string &symbol,
                                       Quantity quantity, Side side, TimeInForce tif);

        void validate() const;
    };

}
