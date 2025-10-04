#include "FeedSimulator.h"
#include <chrono>
#include <cstring>
#include <algorithm>

namespace nse::mtbt {

std::vector<std::uint8_t> FeedSimulator::generateFeed() {
    std::vector<std::uint8_t> feedData;
    feedData.reserve(config_.messageCount * sizeof(TradeMessage));
    
    for (std::uint32_t i = 1; i <= config_.messageCount; ++i) {
        const auto message = generateMessage(i);
        
        // Serialize message
        const auto* messageBytes = reinterpret_cast<const std::uint8_t*>(&message);
        feedData.insert(feedData.end(), messageBytes, messageBytes + sizeof(TradeMessage));
    }
    
    return feedData;
}

std::vector<std::uint8_t> FeedSimulator::generateTestFeed() {
    auto feedData = generateFeed();
    
    if (config_.malformedCount > 0) {
        std::uniform_int_distribution<std::size_t> positionDist(0, feedData.size() - 1);
        std::uniform_int_distribution<std::uint8_t> byteDist(0, 255);
        
        // Inject random errors
        for (std::size_t i = 0; i < config_.malformedCount && i < feedData.size(); ++i) {
            const auto errorPos = positionDist(rng_);
            feedData[errorPos] = byteDist(rng_);
        }
    }
    
    return feedData;
}

TradeMessage FeedSimulator::generateMessage(std::uint32_t sequenceNumber) {
    return TradeMessage{
        sequenceNumber,
        selectSymbolToken(),
        generateTimestamp(),
        generatePrice(),
        generateQuantity(),
        generateTradeSide()
    };
}

std::uint64_t FeedSimulator::generateTimestamp() const {
    const auto now = std::chrono::system_clock::now();
    const auto epoch = now.time_since_epoch();
    const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    
    return static_cast<std::uint64_t>(micros.count());
}

std::uint32_t FeedSimulator::generatePrice() const {
    std::uniform_int_distribution<std::uint32_t> priceDist(5000, 800000); // ₹50 - ₹8000 (realistic NSE range)
    auto basePrice = priceDist(rng_);
    
    // Round to nearest 5 paisa for NSE tick size compliance
    return ((basePrice + 2) / 5) * 5;
}

std::uint32_t FeedSimulator::generateQuantity() const {
    std::uniform_int_distribution<std::uint32_t> quantityDist(1, 10000);
    return quantityDist(rng_);
}

std::uint32_t FeedSimulator::selectSymbolToken() const {
    // Realistic NSE symbol tokens (similar to actual NSE instrument tokens)
    const std::vector<std::uint32_t> symbols = {
        3045,    // SBIN (State Bank of India)
        1270,    // RELIANCE (Reliance Industries)
        11536,   // TCS (Tata Consultancy Services)
        2885,    // INFY (Infosys)
        1594     // HDFCBANK (HDFC Bank)
    };
    std::uniform_int_distribution<std::size_t> indexDist(0, symbols.size() - 1);
    return symbols[indexDist(rng_)];
}

TradeSide FeedSimulator::generateTradeSide() const {
    std::uniform_int_distribution<int> sideDist(0, 1);
    return (sideDist(rng_) == 0) ? TradeSide::BUY : TradeSide::SELL;
}

} // namespace nse::mtbt