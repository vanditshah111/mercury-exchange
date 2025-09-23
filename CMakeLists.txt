cmake_minimum_required(VERSION 3.14)
project(MercuryExchange)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Where to find header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Core sources (excluding main/test drivers)
set(SOURCES_COMMON
    src/Order.cpp
    src/BuyBook.cpp
    src/SellBook.cpp
    src/Market.cpp
    src/MarketRegistry.cpp
    src/MatchingEngine.cpp
    src/Trade.cpp
)

# Build as a library so we can reuse in multiple executables
add_library(mercury_core ${SOURCES_COMMON})
target_include_directories(mercury_core PUBLIC include)

# CLI demo executable
add_executable(mercury_exchange src/main.cpp)
target_link_libraries(mercury_exchange PRIVATE mercury_core)

# Deterministic test executable
add_executable(test_deterministic tests/test_deterministic.cpp)
target_link_libraries(test_deterministic PRIVATE mercury_core)

# Stress test executable
add_executable(test_stress tests/test_stress.cpp)
target_link_libraries(test_stress PRIVATE mercury_core)

add_executable(test_latency tests/test_latency.cpp)
target_link_libraries(test_latency PRIVATE mercury_core)
