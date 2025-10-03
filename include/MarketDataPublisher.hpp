#pragma once
#include "IMarketDataListener.hpp"
#include "MarketEvent.hpp"
#include "blockingconcurrentqueue.h"
#include <vector>
#include <thread>
#include <atomic>
#include <memory>

namespace MercEx {

class MarketDataPublisher {
public:
    MarketDataPublisher();
    ~MarketDataPublisher();

    void publish(const std::vector<MarketEvent>& events);

    void subscribe(IMarketDataListener* listener);

private:
    void run();

    std::vector<IMarketDataListener*> listeners_;
    moodycamel::BlockingConcurrentQueue<std::vector<MarketEvent>> event_queue_;
    
    std::atomic<bool> running_{false};
    std::thread worker_;
};

}