#pragma once
#include <cstdint>
#include "Order.hpp" 

namespace MercEx::Gateway {

enum class MessageType : uint8_t {
    NewOrderRequest = 1,
    NewOrderAck     = 2,
    CancelOrderRequest = 3,
    CancelOrderAck = 4,
    TradeReport     = 5
};


#pragma pack(push, 1)
struct MessageHeader {
    MessageType msg_type;
    uint32_t msg_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NewOrderRequest {
    uint64_t cl_ord_id;
    uint32_t client_id;
    char symbol[8]; 
    Quantity quantity;
    Price price;        
    Side side;
    OrderType order_type;
    TimeInForce tif;
    Price stop_price;   
};
#pragma pack(pop)


#pragma pack(push, 1)
struct NewOrderAck {
    uint64_t cl_ord_id;
    uint32_t client_id;
    OrderID order_id;
    char symbol[8]; 
    Quantity quantity;
    Price price;       
    Side side;
    OrderType order_type;
    TimeInForce tif;
    Price stop_price;   
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CancelOrderRequest {
    uint64_t cl_ord_id;
    uint32_t client_id;
    OrderID order_id;
    char symbol[8]; 
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CancelOrderAck {
    uint64_t cl_ord_id;
    uint32_t client_id;
    OrderID order_id;
    char symbol[8]; 
    bool success;   
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TradeReport {
    char symbol[8];
    OrderID buy_order_id;
    OrderID sell_order_id;
    Price price;
    Quantity quantity;
};
#pragma pack(pop)

} // namespace MercEx::Gateway