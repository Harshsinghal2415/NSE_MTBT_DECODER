#include "Decoder.h"
#include <chrono>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace nse::mtbt {

std::vector<TradeMessage> Decoder::decodeFeed(const std::vector<std::uint8_t>& data) {
    const auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<TradeMessage> messages;
    messages.reserve(data.size() / ProtocolConstants::MESSAGE_SIZE);
    
    std::size_t offset = 0;
    std::uint64_t messageCount = 0;
    
    if (debugMode_) {
        std::cout << "\n=== NSE MTBT Decoder - Real Binary Parsing ===\n";
        std::cout << "Data size: " << data.size() << " bytes\n";
        std::cout << "Expected message size: " << ProtocolConstants::MESSAGE_SIZE << " bytes\n\n";
    }
    
    while (offset + ProtocolConstants::MESSAGE_SIZE <= data.size()) {
        if (auto message = parseBinaryMessage(data.data() + offset, ProtocolConstants::MESSAGE_SIZE); message) {
            if (debugMode_) {
                logBinaryDecoding(*message, data.data() + offset);
            }
            
            const auto validationResult = message->validate();
            if (validationResult.isValid) {
                messages.push_back(*message);
                ++stats_.validMessages;
            } else {
                ++stats_.errorCount;
                if (debugMode_) {
                    std::cout << "❌ Validation failed: " << validationResult.errorMessage.value_or("Unknown error") << "\n";
                }
            }
            ++messageCount;
            offset += ProtocolConstants::MESSAGE_SIZE;
        } else {
            ++stats_.protocolErrors;
            ++stats_.errorCount;
            if (debugMode_) {
                std::cout << "❌ Protocol parsing failed at offset " << offset << "\n";
            }
            ++offset; // Skip bad byte
        }
    }
    
    stats_.truncatedBytes = data.size() - offset;
    updateStats(startTime, offset, messageCount);
    
    if (debugMode_) {
        std::cout << "\n=== Decoding Summary ===\n";
        std::cout << "Messages decoded: " << messageCount << "\n";
        std::cout << "Valid messages: " << stats_.validMessages << "\n";
        std::cout << "Errors: " << stats_.errorCount << "\n";
        std::cout << "Truncated bytes: " << stats_.truncatedBytes << "\n";
    }
    
    return messages;
}

std::optional<TradeMessage> Decoder::parseBinaryMessage(const std::uint8_t* data, std::size_t size) const {
    if (size < ProtocolConstants::MESSAGE_SIZE) {
        return std::nullopt;
    }
    
    // Validate message format first
    if (!validateMessageFormat(data, size)) {
        return std::nullopt;
    }
    
    TradeMessage message;
    
    // Extract fields using real bit-level operations
    message.sequenceNumber = extractUint32(data, ProtocolConstants::OFFSET_SEQUENCE);
    message.symbolToken = extractUint32(data, ProtocolConstants::OFFSET_SYMBOL_TOKEN);
    message.timestamp = extractUint64(data, ProtocolConstants::OFFSET_TIMESTAMP);
    message.priceInPaisa = extractUint32(data, ProtocolConstants::OFFSET_PRICE);
    message.quantity = extractUint32(data, ProtocolConstants::OFFSET_QUANTITY);
    message.side = extractTradeSide(data, ProtocolConstants::OFFSET_SIDE);
    message.checksum = extractUint32(data, ProtocolConstants::OFFSET_CHECKSUM);
    
    // CRC validation if enabled
    if (validationLevel_ >= ValidationLevel::CHECKSUM) {
        const std::uint32_t calculatedCRC = calculateCRC32(data, ProtocolConstants::MESSAGE_SIZE - 4);
        if (calculatedCRC != message.checksum) {
            ++stats_.crcErrors;
            return std::nullopt;
        }
    }
    
    return message;
}

std::uint32_t Decoder::extractUint32(const std::uint8_t* data, std::size_t offset) const {
    // Extract 32-bit value with proper byte order conversion
    std::uint32_t value = 0;
    
    // Little-endian extraction with bit manipulation
    value |= static_cast<std::uint32_t>(data[offset + 0]) << 0;
    value |= static_cast<std::uint32_t>(data[offset + 1]) << 8;
    value |= static_cast<std::uint32_t>(data[offset + 2]) << 16;
    value |= static_cast<std::uint32_t>(data[offset + 3]) << 24;
    
    return value;
}

std::uint64_t Decoder::extractUint64(const std::uint8_t* data, std::size_t offset) const {
    // Extract 64-bit value with proper byte order conversion
    std::uint64_t value = 0;
    
    // Little-endian extraction with bit manipulation
    for (std::size_t i = 0; i < 8; ++i) {
        value |= static_cast<std::uint64_t>(data[offset + i]) << (i * 8);
    }
    
    return value;
}

TradeSide Decoder::extractTradeSide(const std::uint8_t* data, std::size_t offset) const {
    // Extract trade side from single byte
    const std::uint8_t sideValue = data[offset];
    return (sideValue == 0) ? TradeSide::BUY : TradeSide::SELL;
}

std::uint32_t Decoder::calculateCRC32(const std::uint8_t* data, std::size_t size) const {
    // Simplified CRC32 calculation (in real implementation, use proper CRC32)
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

bool Decoder::validateMessageFormat(const std::uint8_t* data, std::size_t size) const {
    if (size < ProtocolConstants::MESSAGE_SIZE) {
        return false;
    }
    
    // Basic sanity checks on the binary data
    // Check if all bytes are not zero (which would indicate corrupted data)
    bool allZero = true;
    for (std::size_t i = 0; i < std::min(size, static_cast<std::size_t>(16)); ++i) {
        if (data[i] != 0) {
            allZero = false;
            break;
        }
    }
    
    return !allZero;
}

void Decoder::logBinaryDecoding(const TradeMessage& message, const std::uint8_t* rawData) const {
    std::cout << "🔍 Binary Message Analysis:\n";
    
    // Show raw hex bytes
    std::cout << "Raw bytes: " << BinaryUtils::toHex(rawData, 16) << "...\n";
    
    // Show token decoding specifically
    const std::uint8_t* tokenBytes = rawData + ProtocolConstants::OFFSET_SYMBOL_TOKEN;
    std::cout << "Token bytes: " << BinaryUtils::toHex(tokenBytes, 4) << "\n";
    std::cout << "Token binary: " << BinaryUtils::toBinary(message.symbolToken) << "\n";
    std::cout << "Token decimal: " << message.symbolToken << "\n";
    std::cout << "Symbol: " << message.getSymbolName() << "\n";
    
    // Show complete message
    std::cout << "Decoded: SEQ=" << message.sequenceNumber 
              << " TOKEN=" << message.symbolToken 
              << " PRICE=₹" << std::fixed << std::setprecision(2) << message.getPriceInRupees()
              << " QTY=" << message.quantity 
              << " SIDE=" << formatTradeSide(message.side) << "\n";
    
    std::cout << "✅ " << message.getDecodingInfo() << "\n\n";
}

void Decoder::updateStats(const std::chrono::high_resolution_clock::time_point& startTime,
                         std::uint64_t processedBytes, std::uint64_t messageCount) noexcept {
    
    const auto endTime = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    stats_.decodedMessages += messageCount;
    stats_.bytesProcessed += processedBytes;
    stats_.totalTimeUs += static_cast<std::uint64_t>(duration.count());
    
    // Calculate processing speed
    if (stats_.totalTimeUs > 0) {
        constexpr std::uint64_t MICROSECONDS_PER_SECOND = 1'000'000;
        stats_.processingSpeed = (stats_.decodedMessages * MICROSECONDS_PER_SECOND) / stats_.totalTimeUs;
    }
}

} // namespace nse::mtbt