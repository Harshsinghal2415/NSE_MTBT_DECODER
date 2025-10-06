#include "FeedSimulator.h"
#include <chrono>
#include <cstring>
#include <algorithm>

namespace nse::mtbt {

std::vector<std::uint8_t> FeedSimulator::generateFeed() {
    return generateBinaryFeed();
}

std::vector<std::uint8_t> FeedSimulator::generateBinaryFeed() {
    std::vector<std::uint8_t> feedData;
    feedData.reserve(config_.messageCount * ProtocolConstants::MESSAGE_SIZE);
    
    for (std::uint32_t i = 1; i <= config_.messageCount; ++i) {
        const auto message = generateMessage(i);
        const auto serializedMessage = serializeMessage(message);
        feedData.insert(feedData.end(), serializedMessage.begin(), serializedMessage.end());
    }
    
    return feedData;
}

std::vector<std::uint8_t> FeedSimulator::generateTestFeed() {
    auto feedData = generateBinaryFeed();
    
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

std::vector<std::uint8_t> FeedSimulator::serializeMessage(const TradeMessage& message) {
    std::vector<std::uint8_t> buffer(ProtocolConstants::MESSAGE_SIZE, 0);
    
    // Serialize each field using bit-level operations
    writeUint32(buffer, ProtocolConstants::OFFSET_SEQUENCE, message.sequenceNumber);
    writeUint32(buffer, ProtocolConstants::OFFSET_SYMBOL_TOKEN, message.symbolToken);
    writeUint64(buffer, ProtocolConstants::OFFSET_TIMESTAMP, message.timestamp);
    writeUint32(buffer, ProtocolConstants::OFFSET_PRICE, message.priceInPaisa);
    writeUint32(buffer, ProtocolConstants::OFFSET_QUANTITY, message.quantity);
    
    // Trade side as single byte
    buffer[ProtocolConstants::OFFSET_SIDE] = static_cast<std::uint8_t>(message.side);
    
    // Calculate and write checksum
    const std::uint32_t checksum = calculateCRC32(buffer.data(), ProtocolConstants::MESSAGE_SIZE - 4);
    writeUint32(buffer, ProtocolConstants::OFFSET_CHECKSUM, checksum);
    
    return buffer;
}

void FeedSimulator::writeUint32(std::vector<std::uint8_t>& buffer, std::size_t offset, std::uint32_t value) {
    // Little-endian byte order
    buffer[offset + 0] = static_cast<std::uint8_t>(value & 0xFF);
    buffer[offset + 1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
    buffer[offset + 2] = static_cast<std::uint8_t>((value >> 16) & 0xFF);
    buffer[offset + 3] = static_cast<std::uint8_t>((value >> 24) & 0xFF);
}

void FeedSimulator::writeUint64(std::vector<std::uint8_t>& buffer, std::size_t offset, std::uint64_t value) {
    // Little-endian byte order
    for (std::size_t i = 0; i < 8; ++i) {
        buffer[offset + i] = static_cast<std::uint8_t>((value >> (i * 8)) & 0xFF);
    }
}

std::uint32_t FeedSimulator::calculateCRC32(const std::uint8_t* data, std::size_t size) const {
    // Simplified CRC32 calculation (matches decoder implementation)
    std::uint32_t crc = 0xFFFFFFFF;
    
    for (std::size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return ~crc;
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
    // Use the same real NSE symbol tokens from SymbolRegistry
    const std::vector<std::uint32_t> symbols = {
        3045,    // SBIN (State Bank of India)
        1270,    // RELIANCE (Reliance Industries)
        11536,   // TCS (Tata Consultancy Services)
        2885,    // INFY (Infosys)
        1594,    // HDFCBANK (HDFC Bank)
        4963,    // ICICIBANK (ICICI Bank)
        8479,    // BHARTIARTL (Bharti Airtel)
        6364,    // KOTAKBANK (Kotak Mahindra Bank)
        1922,    // ITC (ITC Limited)
        5258     // LT (Larsen & Toubro)
    };
    std::uniform_int_distribution<std::size_t> indexDist(0, symbols.size() - 1);
    return symbols[indexDist(rng_)];
}

TradeSide FeedSimulator::generateTradeSide() const {
    std::uniform_int_distribution<int> sideDist(0, 1);
    return (sideDist(rng_) == 0) ? TradeSide::BUY : TradeSide::SELL;
}

} // namespace nse::mtbt