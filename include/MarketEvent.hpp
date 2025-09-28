#pragma once
#include <cstdint>
#include <optional>
#include "Order.hpp"

namespace MercEx
{

    enum class MarketEventType : uint8_t
    {
        AddOrder,
        FilledOrder,
        CancelOrder,
        Trade,
        StopTriggered
    };

    struct MarketEvent
    {
        MarketEventType type;

        TimePoint timestamp;
        OrderID order_id;
        ClientID client_id;
        std::string symbol;
        Quantity quantity;
        Side side;
        std::optional<Price> price;
        std::optional<Price> stop_price;
        std::optional<OrderType> order_type;
        std::optional<TimeInForce> tif;

        std::optional<OrderID> counterparty_id;
        std::optional<Price> executed_price;
        std::optional<Quantity> executed_qty;

        static MarketEvent make_add(OrderID id, ClientID client_id, const std::string &symbol,
                                    Quantity quantity, Side side,
                                    std::optional<Price> price = std::nullopt,
                                    std::optional<Price> stop_price = std::nullopt,
                                    OrderType type = OrderType::Limit,
                                    TimeInForce tif = TimeInForce::Day)
        {
            return {MarketEventType::AddOrder, std::chrono::steady_clock::now(), id, client_id, symbol, quantity, side,
                    price, stop_price, type, tif,
                    std::nullopt, std::nullopt, std::nullopt};
        }

        static MarketEvent make_filled(OrderID id, const std::string &symbol)
        {
            return {MarketEventType::FilledOrder, std::chrono::steady_clock::now(), id, 0, symbol, 0, Side::Buy,
                    std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                    std::nullopt, std::nullopt, std::nullopt};
        }

        static MarketEvent make_cancel(OrderID id, const std::string &symbol)
        {
            return {MarketEventType::CancelOrder, std::chrono::steady_clock::now(), id, 0, symbol, 0, Side::Buy,
                    std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                    std::nullopt, std::nullopt, std::nullopt};
        }

        static MarketEvent make_trade(OrderID id, OrderID counterparty_id,
                                      Price trade_price, Quantity trade_qty)
        {
            return {MarketEventType::Trade, std::chrono::steady_clock::now(), id, 0, "", 0, Side::Buy,
                    std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                    counterparty_id, trade_price, trade_qty};
        }

        static MarketEvent make_stop_triggered(OrderID id, ClientID client_id, const std::string &symbol,
                                               Quantity quantity, Side side,
                                               Price stop_price,
                                               OrderType type = OrderType::Stop,
                                               TimeInForce tif = TimeInForce::Day)
        {
            return {MarketEventType::StopTriggered, std::chrono::steady_clock::now(), id, client_id, symbol, quantity, side,
                    std::nullopt, stop_price, type, tif,
                    std::nullopt, std::nullopt, std::nullopt};
        }
    };

}
