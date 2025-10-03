#include "MarketDataPublisher.hpp"

namespace MercEx {

MarketDataPublisher::MarketDataPublisher() {
    running_.store(true);
    worker_ = std::thread(&MarketDataPublisher::run, this);
}

MarketDataPublisher::~MarketDataPublisher() {
    running_.store(false);
    event_queue_.enqueue({}); 
    if (worker_.joinable()) {
        worker_.join();
    }
}

void MarketDataPublisher::publish(const std::vector<MarketEvent>& events) {
    if (!events.empty()) {
        event_queue_.enqueue(events);
    }
}

void MarketDataPublisher::subscribe(IMarketDataListener* listener) {
    if (listener) {
        listeners_.push_back(listener);
    }
}

void MarketDataPublisher::run() {
    std::vector<MarketEvent> events_to_process;
    while (running_.load()) {
        event_queue_.wait_dequeue(events_to_process);

        if (!running_.load() && events_to_process.empty()) {
            break;
        }

        for (IMarketDataListener* listener : listeners_) {
            listener->on_market_events(events_to_process);
        }
        events_to_process.clear();
    }
}

}