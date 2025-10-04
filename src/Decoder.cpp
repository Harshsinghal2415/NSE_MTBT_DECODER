#include "Decoder.h"
#include <chrono>
#include <cstring>
#include <algorithm>

namespace nse::mtbt {

std::vector<TradeMessage> Decoder::decodeFeed(const std::vector<std::uint8_t>& data) {
    const auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<TradeMessage> messages;
    messages.reserve(data.size() / sizeof(TradeMessage));
    
    std::size_t offset = 0;
    std::uint64_t messageCount = 0;
    
    while (offset + sizeof(TradeMessage) <= data.size()) {
        if (auto message = parseMessage(data.data() + offset, sizeof(TradeMessage)); message) {
            const auto validationResult = message->validate();
            if (validationResult.isValid) {
                messages.push_back(*message);
                ++stats_.validMessages;
            } else {
                ++stats_.errorCount;
            }
            ++messageCount;
            offset += sizeof(TradeMessage);
        } else {
            ++stats_.errorCount;
            ++offset; // Skip bad byte
        }
    }
    
    stats_.truncatedBytes = data.size() - offset;
    updateStats(startTime, offset, messageCount);
    
    return messages;
}

std::optional<TradeMessage> Decoder::parseMessage(const std::uint8_t* data, std::size_t size) const {
    if (size < sizeof(TradeMessage)) {
        return std::nullopt;
    }
    
    TradeMessage message;
    std::memcpy(&message, data, sizeof(TradeMessage));
    
    return message;
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