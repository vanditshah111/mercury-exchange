#include "../include/Trade.hpp"
#include <sstream>

namespace MercEx
{

    Trade::Trade(TradeID id,
                 OrderID buy_order_id,
                 OrderID sell_order_id,
                 ClientID buyer_id,
                 ClientID seller_id,
                 double price,
                 int quantity)
        : id_(id),
          buy_order_id_(buy_order_id),
          sell_order_id_(sell_order_id),
          buyer_id_(buyer_id),
          seller_id_(seller_id),
          price_(price),
          quantity_(quantity) {}

    TradeID Trade::get_id() const { return id_; }
    OrderID Trade::get_buy_order_id() const { return buy_order_id_; }
    OrderID Trade::get_sell_order_id() const { return sell_order_id_; }
    ClientID Trade::get_buyer_id() const { return buyer_id_; }
    ClientID Trade::get_seller_id() const { return seller_id_; }
    Price Trade::get_price() const { return price_; }
    Quantity Trade::get_quantity() const { return quantity_; }

    std::string Trade::to_string() const
    {
        std::ostringstream oss;
        oss << "Trade[ID=" << id_
            << ", BuyOrder=" << buy_order_id_
            << ", SellOrder=" << sell_order_id_
            << ", Buyer=" << buyer_id_
            << ", Seller=" << seller_id_
            << ", Price=" << price_
            << ", Quantity=" << quantity_
            << "]";
        return oss.str();
    }

} // namespace MercEx
