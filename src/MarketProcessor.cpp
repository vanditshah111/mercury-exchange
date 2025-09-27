#include "../include/MarketProcessor.hpp"
#include <iostream>

namespace MercEx
{

    MarketProcessor::MarketProcessor(std::unique_ptr<Market> m)
        : market(std::move(m)), running(true)
    {
        worker = std::thread(&MarketProcessor::run, this);
    }
    MarketProcessor::~MarketProcessor() { stop(); }

    void MarketProcessor::submit_event(const MarketEvent &ev)
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push_back(ev);
        }
        cv.notify_one();
    }

    void MarketProcessor::run()
    {
        while (true)
        {
            MarketEvent ev;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]
                        { return !queue.empty() || !running; });
                if (!running && queue.empty())
                    break;
                ev = queue.front();
                queue.pop_front();
            }
            std::cout << "Processing some event: " << std::endl;
            handle_event(ev);
        }
    }

    void MarketProcessor::stop()
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            running = false;
        }
        cv.notify_all();
        if (worker.joinable())
        {
            worker.join();
        }
    }

    void MarketProcessor::handle_event(MarketEvent &ev)
    {
        switch (ev.type)
        {
        case MarketEventType::AddOrder:
        {
            std::unique_ptr<Order> order;
            if (ev.order_type.value() == OrderType::Limit)
            {
                order = Order::make_limit_order(ev.order_id, ev.client_id, ev.symbol,
                                                ev.quantity, ev.price.value(), ev.side, ev.tif.value());
            }
            else if (ev.order_type.value() == OrderType::Market)
            {
                order = Order::make_market_order(ev.order_id, ev.client_id, ev.symbol,
                                                 ev.quantity, ev.side, ev.tif.value());
            }
            else if (ev.order_type.value() == OrderType::Stop)
            {
                order = Order::make_stop_order(ev.order_id, ev.client_id, ev.symbol,
                                               ev.quantity, ev.stop_price.value(), ev.side, ev.tif.value());
            }
            else if (ev.order_type.value() == OrderType::StopLimit)
            {
                order = Order::make_stop_limit_order(ev.order_id, ev.client_id, ev.symbol,
                                                     ev.quantity, ev.price.value(), ev.stop_price.value(), ev.side, ev.tif.value());
            }

            OrderID id = order->id;
            orders_local_[id] = std::move(order);
            Order *ord_ptr = orders_local_[id].get();

            if (ord_ptr->type == OrderType::Stop || ord_ptr->type == OrderType::StopLimit)
            {
                if (ord_ptr->side == Side::Buy)
                {
                    stop_buy_orders_[*ord_ptr->stop_price].push_back(ord_ptr);
                }
                else
                {
                    stop_sell_orders_[*ord_ptr->stop_price].push_back(ord_ptr);
                }
                return;
            }

            Price prevltp = market->get_last_price().value_or(0.0);
            auto events = market->process_order(*ord_ptr);
            handle_market_events(events);
            if (market->get_last_price().value_or(0.0) != prevltp)
            {
                check_stop_orders();
            }
            break;
        }

        case MarketEventType::CancelOrder:
        {
            auto it = orders_local_.find(ev.order_id);
            if (it != orders_local_.end())
            {
                Order *ord = it->second.get();
                market->cancel_order(ord);
                ord->status = OrderStatus::Canceled;
            }
            break;
        }
        }
    }

    void MarketProcessor::check_stop_orders()
    {
        auto ltp_opt = market->get_last_price();
        if (!ltp_opt)
            return;
        Price ltp = *ltp_opt;

        while (!stop_buy_orders_.empty() && market->get_last_price().value() >= stop_buy_orders_.begin()->first)
        {
            auto price_level = stop_buy_orders_.begin();
            for (auto *ord : price_level->second)
            {
                ord->type = (ord->type == OrderType::Stop) ? OrderType::Market : OrderType::Limit;
                auto events = market->process_order(*ord);
                handle_market_events(events);
            }
            stop_buy_orders_.erase(price_level);
        }

        while (!stop_sell_orders_.empty() && market->get_last_price().value() <= stop_sell_orders_.begin()->first)
        {
            auto price_level = stop_sell_orders_.begin();
            for (auto *ord : price_level->second)
            {
                ord->type = (ord->type == OrderType::Stop) ? OrderType::Market : OrderType::Limit;
                auto events = market->process_order(*ord);
                handle_market_events(events);
            }
            stop_sell_orders_.erase(price_level);
        }
    }

    void MarketProcessor::handle_market_events(const std::vector<MarketEvent> &events)
    {
        for (const auto &e : events)
        {
            std::cout << "Market Event: Type=" << static_cast<int>(e.type)
                      << ", OrderID=" << e.order_id
                      << ", Quantity=" << e.quantity
                      << ", Price=" << (e.price.has_value() ? std::to_string(*e.price) : "N/A")
                      << ", StopPrice=" << (e.stop_price.has_value() ? std::to_string(*e.stop_price) : "N/A")
                      << ", Side=" << static_cast<int>(e.side)
                      << ", ExecutedQty=" << (e.executed_qty.has_value() ? std::to_string(*e.executed_qty) : "N/A")
                      << ", ExecutedPrice=" << (e.executed_price.has_value() ? std::to_string(*e.executed_price) : "N/A")
                      << std::endl;
        }
    }

    Market &MarketProcessor::get_market()
    {
        return *market;
    }

    MarketID MarketProcessor::get_market_id() const
    {
        return market->get_market_id();
    }
} // namespace MercEx
