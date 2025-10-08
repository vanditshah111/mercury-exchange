#include "../include/ClientSession.hpp"

ClientSession::ClientSession(tcp::socket socket, MercEx::MatchingEngine &engine, MercEx::MarketDataPublisher &publisher)
    : socket_(std::move(socket)),
      engine_(engine),
      publisher_(publisher) {}

ClientSession::~ClientSession()
{
    // In a real system, you would add a method to publisher_ to unsubscribe this session.
    // For now, this is sufficient.
    std::cout << "Client session destroyed." << std::endl;
}

void ClientSession::start()
{
    std::cout << "Client session started." << std::endl;
    // Subscribe to the publisher to receive market data updates.
    publisher_.subscribe(this);
    // Start the first async read from the client.
    do_read();
}

// This method is the entry point for data coming FROM the exchange TO the client.
void ClientSession::on_market_events(const std::vector<MercEx::MarketEvent> &events)
{
    bool write_in_progress = !write_msgs_.empty();
    for (const auto &event : events)
    {
        std::string msg = MercEx::MarketEvent::event_to_string(event);
        std::cout << msg;
        write_msgs_.push_back(msg);
    }

    // If a write operation wasn't already in progress, start one.
    if (!write_in_progress && !write_msgs_.empty())
    {
        do_write();
    }
}

// Handles incoming data FROM the client TO the exchange.
void ClientSession::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(read_data_, max_length),
                            [this, self](boost::system::error_code ec, std::size_t length)
                            {
                                if (!ec)
                                {
                                    std::string received_msg(read_data_, length);
                                    std::cout << "Received from client: " << received_msg;

                                    std::stringstream ss(received_msg);
                                    std::string segment;
                                    std::vector<std::string> segments;
                                    while (std::getline(ss, segment, ','))
                                    {
                                        segments.push_back(segment);
                                    }
                                    if (segments.size() >= 5 && segments[0] == "Limit")
                                    {
                                        MercEx::OrderType order_type = MercEx::OrderType::Limit;
                                        MercEx::Side side = (segments[1] == "BUY") ? MercEx::Side::Buy : MercEx::Side::Sell;
                                        std::string symbol = segments[2];
                                        int quantity = std::stoi(segments[3]);
                                        double price = std::stod(segments[4]);
                                        // For simplicity, we use a fixed order ID and TimeInForce here.
                                        
                                        engine_.submit_order(1, symbol, quantity, side, price, order_type, MercEx::TimeInForce::GTC);
                                    }
                                    else if (segments.size() >= 5 && segments[0] == "Market")
                                    {
                                        MercEx::OrderType order_type = MercEx::OrderType::Market;
                                        MercEx::Side side = (segments[1] == "BUY") ? MercEx::Side::Buy : MercEx::Side::Sell;
                                        std::string symbol = segments[2];
                                        int quantity = std::stoi(segments[3]);
                                        double price = std::stod(segments[4]);
                                        engine_.submit_order(1, symbol, quantity, side, price, order_type, MercEx::TimeInForce::GTC);
                                    }
                                    else if(segments.size() >= 6 && segments[0] == "Stop")
                                    {
                                        MercEx::OrderType order_type = MercEx::OrderType::Stop;
                                        MercEx::Side side = (segments[1] == "BUY") ? MercEx::Side::Buy : MercEx::Side::Sell;
                                        std::string symbol = segments[2];
                                        int quantity = std::stoi(segments[3]);
                                        double stop_price = std::stod(segments[5]);
                                        engine_.submit_order(1, symbol, quantity, side, std::nullopt, order_type, MercEx::TimeInForce::GTC, stop_price);
                                    }
                                    else if(segments.size() >= 5 && segments[0] == "StopLimit")
                                    {
                                        MercEx::OrderType order_type = MercEx::OrderType::StopLimit;
                                        MercEx::Side side = (segments[1] == "BUY") ? MercEx::Side::Buy : MercEx::Side::Sell;
                                        std::string symbol = segments[2];
                                        int quantity = std::stoi(segments[3]);
                                        double price = std::stod(segments[4]);
                                        double stop_price = std::stod(segments[5]);
                                        engine_.submit_order(1, symbol, quantity, side, price, order_type, MercEx::TimeInForce::GTC, stop_price);
                                    }
                                    else if (segments.size() >= 3 && segments[0] == "CANCEL")
                                    {
                                        std::string symbol = segments[1];
                                        uint64_t order_id = std::stoull(segments[2]);
                                        bool success = engine_.cancel_order(order_id, symbol);
                                        if (!success)
                                        {
                                            std::cout << "Failed to cancel order ID " << order_id << " for symbol " << symbol << std::endl;
                                        }
                                    }
                                    else
                                    {
                                        std::cout << "Unrecognized command from client." << std::endl;
                                    }

                                    // After processing, continue reading the next message from the client.
                                    do_read();
                                }
                                else
                                {
                                    // If an error occurs (like a disconnect), the session will be destroyed.
                                }
                            });
}

// Handles writing outgoing data TO the client.
void ClientSession::do_write()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/)
                             {
                                 if (!ec)
                                 {
                                     write_msgs_.pop_front();
                                     if (!write_msgs_.empty())
                                     {
                                         // Continue writing if there are more messages in the queue.
                                         do_write();
                                     }
                                 }
                             });
}