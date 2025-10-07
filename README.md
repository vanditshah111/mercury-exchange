# Mercury Exchange  

Mercury Exchange is a lightweight, high-performance matching engine written in modern C++17.  
The goal of this project is to be fast, simple, and reliable — something you can use for research, trading simulations, or just to explore how real exchanges work under the hood.  

## How it works  
- Each market (AAPL, GOOG, MSFT, etc.) runs in its own thread. This avoids locks on the hot path and makes it easy to scale across multiple markets.  
- Orders and events move through lock-free queues, so the matching loop is never blocked.  
- Market data (trades, fills, book updates) is published on a separate thread, so slow consumers can’t hold up the engine.  

## Features  
- **Ultra-low latency** – Handles over **2.3M orders per second** in benchmarks, with average latency around **1.1 microseconds** in release builds.  
- **Multiple markets** – Heavy volume in one symbol doesn’t affect the others.  
- **Order types** – Supports limit, market, stop, and stop-limit orders with time-in-force (GTC, IOC, FOK).  
- **Async market data feed** – Subscribers get events without slowing down the core.  
- **Tested** – Comes with a GoogleTest suite covering matching rules, partial/multi-fills, stop triggers, and more.  

---
