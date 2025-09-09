#pragma once

#include <string>
#include "Order.hpp"

namespace MercEx
{

    using TradeID = int;

    class Trade
    {
    public:
        Trade(TradeID id,
              OrderID buy_order_id,
              OrderID sell_order_id,
              ClientID buyer_id,
              ClientID seller_id,
              double price,
              int quantity);

        TradeID get_id() const;
        OrderID get_buy_order_id() const;
        OrderID get_sell_order_id() const;
        ClientID get_buyer_id() const;
        ClientID get_seller_id() const;
        double get_price() const;
        int get_quantity() const;

        std::string to_string() const;

    private:
        TradeID id_;
        OrderID buy_order_id_;
        OrderID sell_order_id_;
        ClientID buyer_id_;
        ClientID seller_id_;
        Price price_;
        Quantity quantity_;
    };

} // namespace MercEx
