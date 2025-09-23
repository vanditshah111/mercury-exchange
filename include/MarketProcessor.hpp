#include "Market.hpp"
#include "MarketEvent.hpp"

namespace MercEx {

    class MarketProcessor {
        public:

        void submit_order(Order& order);
        
        private:
        std::multimap<Price,Order*> stop_buy_orders_;
        std::multimap<Price,Order*> stop_sell_orders_;
        std::multimap<Price,Order*> stop_limit_buy_orders_;
        std::multimap<Price,Order*> stop_limit_sell_orders_;

    };
}