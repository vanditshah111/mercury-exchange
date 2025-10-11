#include "../include/MatchingEngine.hpp"
#include "../include/MarketRegistry.hpp"
#include "../include/MarketDataPublisher.hpp"
#include "../include/TCPServer.hpp"
#include <iostream>

using namespace MercEx;

int main() {
    try {
        MarketDataPublisher publisher;
        MarketRegistry registry(publisher);
        MatchingEngine engine(registry);

        registry.create_market("AAPL", 0.01, 1);
        registry.create_market("GOOG", 0.01, 2);

        boost::asio::io_context io_context;
        TCPServer server(io_context, 8080, engine, publisher);

        io_context.run();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}