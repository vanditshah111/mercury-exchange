#include "../include/MarketProcessor.hpp"
#include <iostream>
#include <chrono>

namespace MercEx
{
    MarketProcessor::MarketProcessor(std::unique_ptr<Market> m)
        : market(std::move(m)) {}

    MarketProcessor::~MarketProcessor()
    {
        stop();
        if (worker.joinable())
            worker.join();
    }

    void MarketProcessor::start()
    {
        running.store(true, std::memory_order_release);
        worker = std::thread(&MarketProcessor::run, this);
    }

    void MarketProcessor::stop()
    {
        running.store(false, std::memory_order_release);
    }

    void MarketProcessor::submit_event(const MarketEvent &ev)
    {
        queue.enqueue(ev);
    }

    void MarketProcessor::run()
    {
        MarketEvent ev;
        while (running.load(std::memory_order_acquire))
        {
            if (queue.try_dequeue(ev))
            {
                try
                {
                    handle_event(ev);
                }
                catch (const std::exception &e)
                {
                    std::cerr << "[Worker Exception] " << e.what() << std::endl;
                }
                catch (...)
                {
                    std::cerr << "[Worker Exception] unknown" << std::endl;
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::nanoseconds(100));
            }
        }
    }

    void MarketProcessor::handle_event(MarketEvent &ev)
    {
        auto submit_time = std::chrono::steady_clock::now();
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

            auto end = std::chrono::steady_clock::now();
            auto latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - submit_time).count();

            total_latency_ns += latency_ns;
            processed_orders++;

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

        while (!stop_buy_orders_.empty() && ltp >= stop_buy_orders_.begin()->first)
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

        while (!stop_sell_orders_.empty() && ltp <= stop_sell_orders_.begin()->first)
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
        // hook for logging, metrics, etc.
        // disabled now for perf
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
