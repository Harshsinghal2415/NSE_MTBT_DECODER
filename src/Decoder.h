#pragma once

#include "MessageTypes.h"
#include <vector>
#include <optional>
#include <chrono>

namespace nse::mtbt {

/**
 * High-performance message decoder
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
     * Decode messages from binary data
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

private:
    DecodingStats stats_{};
    ValidationLevel validationLevel_{ValidationLevel::STRICT};
    
    // Helper methods
    [[nodiscard]] std::optional<TradeMessage> parseMessage(const std::uint8_t* data, std::size_t size) const;
    void updateStats(const std::chrono::high_resolution_clock::time_point& startTime,
                    std::uint64_t processedBytes, std::uint64_t messageCount) noexcept;
};

} // namespace nse::mtbt