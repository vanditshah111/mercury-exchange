cmake_minimum_required(VERSION 3.14)
project(MercuryExchange)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Where to find header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect all cpp files in src/
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
set(SOURCES
    src/main.cpp
    src/Order.cpp
    src/BuyBook.cpp
    src/SellBook.cpp
    src/Market.cpp
    src/MarketRegistry.cpp
    src/MatchingEngine.cpp
    src/Trade.cpp
)
add_executable(mercury_exchange ${SOURCES})
target_include_directories(mercury_exchange PRIVATE include)

# Define the executable
#add_executable(mercury_exchange ${SOURCES})
