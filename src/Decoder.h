#pragma once

#include "MessageTypes.h"
#include <vector>
#include <optional>
#include <chrono>

namespace nse::mtbt {

/**
 * High-performance message decoder with real bit-level decoding
 */
class Decoder {
public:
    /**
     * Comprehensive statistics for performance monitoring
     */
    struct DecodingStats {
        std::uint64_t decodedMessages{0};
        std::uint64_t validMessages{0};
        std::uint64_t errorCount{0};
        std::uint64_t bytesProcessed{0};
        std::uint64_t truncatedBytes{0};
        std::uint64_t totalTimeUs{0};
        std::uint64_t processingSpeed{0}; // messages per second
        std::uint64_t crcErrors{0};       // CRC validation errors
        std::uint64_t protocolErrors{0};  // Protocol format errors
    };

    /**
     * Modern constructor
     */
    explicit Decoder() noexcept = default;
    
    Decoder(const Decoder&) = delete;
    Decoder& operator=(const Decoder&) = delete;
    Decoder(Decoder&&) noexcept = default;
    Decoder& operator=(Decoder&&) noexcept = default;
    ~Decoder() = default;

    /**
     * Decode messages from binary data using real bit-level parsing
     */
    [[nodiscard]] std::vector<TradeMessage> decodeFeed(const std::vector<std::uint8_t>& data);

    /**
     * Get comprehensive decoding statistics
     */
    [[nodiscard]] const DecodingStats& getStats() const noexcept { return stats_; }

    /**
     * Reset decoder state
     */
    void reset() noexcept { stats_ = DecodingStats{}; }

    /**
     * Set validation level
     */
    void setValidationLevel(ValidationLevel level) noexcept { validationLevel_ = level; }

    /**
     * Enable/disable debug output
     */
    void setDebugMode(bool enabled) noexcept { debugMode_ = enabled; }

private:
    mutable DecodingStats stats_{};
    ValidationLevel validationLevel_{ValidationLevel::STRICT};
    bool debugMode_{false};
    
    // Real bit-level parsing methods
    [[nodiscard]] std::optional<TradeMessage> parseBinaryMessage(const std::uint8_t* data, std::size_t size) const;
    [[nodiscard]] std::uint32_t extractUint32(const std::uint8_t* data, std::size_t offset) const;
    [[nodiscard]] std::uint64_t extractUint64(const std::uint8_t* data, std::size_t offset) const;
    [[nodiscard]] TradeSide extractTradeSide(const std::uint8_t* data, std::size_t offset) const;
    [[nodiscard]] std::uint32_t calculateCRC32(const std::uint8_t* data, std::size_t size) const;
    [[nodiscard]] bool validateMessageFormat(const std::uint8_t* data, std::size_t size) const;
    
    void updateStats(const std::chrono::high_resolution_clock::time_point& startTime,
                    std::uint64_t processedBytes, std::uint64_t messageCount) noexcept;
    void logBinaryDecoding(const TradeMessage& message, const std::uint8_t* rawData) const;
};

} // namespace nse::mtbt