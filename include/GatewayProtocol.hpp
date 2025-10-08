#pragma once
#include <cstdint>
#include "Order.hpp" // For enums like Side, OrderType

namespace MercEx::Gateway {

// --- Message Type Enum ---
enum class MessageType : uint8_t {
    NewOrderRequest = 1,
    NewOrderAck     = 2,
    CancelOrderRequest = 3,
    CancelOrderAck = 4,
    TradeReport     = 5
};

// --- Message Header (Fixed Size, 5 bytes total) ---
// This tells us what kind of message is coming and how big its payload is.
#pragma pack(push, 1) // Ensures compiler doesn't add padding bytes
struct MessageHeader {
    MessageType msg_type;
    uint32_t msg_size;
};
#pragma pack(pop)

// --- Payloads (Client -> Server) ---
#pragma pack(push, 1)
struct NewOrderRequest {
    uint64_t cl_ord_id;
    uint32_t client_id;
    char symbol[8]; // Using fixed-size char array for binary compatibility
    Quantity quantity;
    Price price;        // Price is a double (8 bytes)
    Side side;
    OrderType order_type;
    TimeInForce tif;
    Price stop_price;   // Use 0 if not a stop order
};
#pragma pack(pop)

// --- Payloads (Server -> Client) ---
#pragma pack(push, 1)
struct NewOrderAck {
    uint64_t cl_ord_id;
    uint32_t client_id;
    OrderID order_id;
    char symbol[8]; // Using fixed-size char array for binary compatibility
    Quantity quantity;
    Price price;        // Price is a double (8 bytes)
    Side side;
    OrderType order_type;
    TimeInForce tif;
    Price stop_price;   // Use 0 if not a stop order
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CancelOrderRequest {
    uint64_t cl_ord_id;
    uint32_t client_id;
    OrderID order_id;
    char symbol[8]; // Using fixed-size char array for binary compatibility
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CancelOrderAck {
    uint64_t cl_ord_id;
    uint32_t client_id;
    OrderID order_id;
    char symbol[8]; // Using fixed-size char array for binary compatibility
    bool success;   // true if cancellation succeeded, false otherwise
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