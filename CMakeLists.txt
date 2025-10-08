cmake_minimum_required(VERSION 3.14)
project(MercuryExchange)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Thread flag for non-Windows ---
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# --- Find Boost headers and system library ---
find_package(Boost 1.70 REQUIRED COMPONENTS system)

# --- Include Project Headers ---
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS})  # Add Boost headers

# --- Core Library (no networking) ---
set(SOURCES_CORE
    src/Order.cpp
    src/BuyBook.cpp
    src/SellBook.cpp
    src/Market.cpp
    src/MarketRegistry.cpp
    src/MatchingEngine.cpp
    src/MarketProcessor.cpp
    src/Trade.cpp
    src/MarketDataPublisher.cpp
)
add_library(mercury_core ${SOURCES_CORE})
target_include_directories(mercury_core PUBLIC include)

# --- Gateway Library (handles networking) ---
set(SOURCES_GATEWAY
    src/TCPServer.cpp
    src/ClientSession.cpp
)
add_library(mercury_gateway ${SOURCES_GATEWAY})
target_include_directories(mercury_gateway PRIVATE include ${Boost_INCLUDE_DIRS})
target_link_libraries(mercury_gateway PRIVATE
    mercury_core
    Boost::system
)
target_compile_definitions(mercury_gateway PRIVATE ASIO_STANDALONE)

# Windows: link Winsock library
if(WIN32)
    target_link_libraries(mercury_gateway PRIVATE ws2_32)
endif()

# --- Server Executable ---
add_executable(exchange_server src/ExchangeServer.cpp)
target_link_libraries(exchange_server PRIVATE mercury_gateway)
set_target_properties(exchange_server PROPERTIES WIN32_EXECUTABLE FALSE)

# --- Client Executable ---
add_executable(simple_client src/simple_client.cpp)
target_include_directories(simple_client PRIVATE include ${Boost_INCLUDE_DIRS})
target_link_libraries(simple_client PRIVATE Boost::system)
target_compile_definitions(simple_client PRIVATE ASIO_STANDALONE)
if(WIN32)
    target_link_libraries(simple_client PRIVATE ws2_32)
endif()
