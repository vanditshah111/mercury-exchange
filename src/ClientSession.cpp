#include "../include/ClientSession.hpp"
#include "../include/GatewayProtocol.hpp" 

ClientSession::ClientSession(tcp::socket socket, MercEx::MatchingEngine &engine, MercEx::MarketDataPublisher &publisher)
    : socket_(std::move(socket)),
      engine_(engine),
      publisher_(publisher) {}

ClientSession::~ClientSession() { std::cout << "Client session destroyed." << std::endl; }

void ClientSession::start()
{
    socket_.set_option(boost::asio::ip::tcp::no_delay(true));
    publisher_.subscribe(this);
    do_read_header(); 
}

void ClientSession::on_market_events(const std::vector<MercEx::MarketEvent> &events)
{
    auto self(shared_from_this());
    boost::asio::post(socket_.get_executor(), [this, self, events]()
                      {
        bool write_in_progress = !write_msgs_.empty();
        for (const auto &event : events) {

            if(event.type == MercEx::MarketEventType::AddOrder)
            {
                std::vector<char> buffer(sizeof(MercEx::Gateway::MessageHeader) + sizeof(MercEx::Gateway::NewOrderAck));
                auto* header = reinterpret_cast<MercEx::Gateway::MessageHeader*>(buffer.data());
                header->msg_type = MercEx::Gateway::MessageType::NewOrderAck;
                header->msg_size = sizeof(MercEx::Gateway::NewOrderAck);

                auto* req = reinterpret_cast<MercEx::Gateway::NewOrderAck*>(buffer.data() + sizeof(MercEx::Gateway::MessageHeader));
                req->client_id = event.client_id;
                req->cl_ord_id = event.order_id;
                req->order_id = event.order_id;
                std::strncpy(req->symbol, event.symbol.c_str(), sizeof(req->symbol) - 1);
                req->quantity = event.quantity;
                req->price = event.price.value_or(0.0);
                req->side = event.side;
                req->order_type = event.order_type.value_or(MercEx::OrderType::Limit);
                req->tif = event.tif.value_or(MercEx::TimeInForce::Day);
                req->stop_price = event.stop_price.value_or(0.0);

                write_msgs_.push_back(std::move(buffer));
            }
            else if (event.type == MercEx::MarketEventType::Trade) {
                std::vector<char> buffer(sizeof(MercEx::Gateway::MessageHeader) + sizeof(MercEx::Gateway::TradeReport));
                auto* header = reinterpret_cast<MercEx::Gateway::MessageHeader*>(buffer.data());
                header->msg_type = MercEx::Gateway::MessageType::TradeReport;
                header->msg_size = sizeof(MercEx::Gateway::TradeReport);

                auto* report = reinterpret_cast<MercEx::Gateway::TradeReport*>(buffer.data() + sizeof(MercEx::Gateway::MessageHeader));
                std::strncpy(report->symbol, event.symbol.c_str(), sizeof(report->symbol) - 1);
                report->price = *event.executed_price;
                report->quantity = *event.executed_qty;
                write_msgs_.push_back(std::move(buffer));
            }
        }

        if (!write_in_progress) {
            do_write();
        } });
}

void ClientSession::do_read_header()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(&read_header_, sizeof(MercEx::Gateway::MessageHeader)),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                if (!ec)
                                {
                                    do_read_body();
                                }
                            });
}

void ClientSession::do_read_body()
{
    read_body_.resize(read_header_.msg_size);
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_body_.data(), read_body_.size()),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                if (!ec)
                                {
                                    process_binary_message();
                                    do_read_header();
                                }
                            });
}

void ClientSession::process_binary_message()
{
    try
    {
        if (read_header_.msg_type == MercEx::Gateway::MessageType::NewOrderRequest)
        {
            const auto *req = reinterpret_cast<const MercEx::Gateway::NewOrderRequest *>(read_body_.data());

            std::cout << "[Server] Received Binary NewOrderRequest for " << req->symbol << std::endl;

            engine_.submit_order(
                req->client_id,
                req->symbol,
                req->quantity,
                req->side,
                req->order_type == MercEx::OrderType::Market ? std::nullopt : std::optional<MercEx::Price>(req->price),
                req->order_type,
                req->tif,
                req->stop_price == 0 ? std::nullopt : std::optional<MercEx::Price>(req->stop_price));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error processing binary message: " << e.what() << std::endl;
    }
}

void ClientSession::do_write()
{
    if (write_msgs_.empty())
        return;
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().size()),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/)
                             {
                                 if (!ec)
                                 {
                                     write_msgs_.pop_front();
                                     if (!write_msgs_.empty())
                                     {
                                         do_write();
                                     }
                                 }
                                 else
                                 {
                                     std::cerr << "[ClientSession] Write error: " << ec.message() << std::endl;
                                     write_msgs_.clear();
                                 }
                             });
}