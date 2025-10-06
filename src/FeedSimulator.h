#pragma once

#include "MessageTypes.h"
#include <vector>
#include <random>
#include <optional>

namespace nse::mtbt {

/**
 * Feed simulator for NSE MTBT data
 */
class FeedSimulator {
public:
    /**
     * Configuration for feed generation
     */
    struct Config {
        std::size_t messageCount{1000};
        std::size_t malformedCount{0};
        std::uint32_t seed{0};
        ValidationLevel validationLevel{ValidationLevel::STRICT};
        
        Config() = default;
    };

    /**
     * Default constructor
     */
    FeedSimulator() : FeedSimulator(Config{}) {}

    /**
     * Modern constructor
     */
    explicit FeedSimulator(Config config) : config_{std::move(config)} {
        if (config_.seed == 0) {
            config_.seed = std::random_device{}();
        }
        rng_.seed(config_.seed);
    }

    FeedSimulator(const FeedSimulator&) = delete;
    FeedSimulator& operator=(const FeedSimulator&) = delete;
    FeedSimulator(FeedSimulator&&) noexcept = default;
    FeedSimulator& operator=(FeedSimulator&&) noexcept = default;
    ~FeedSimulator() = default;

    /**
     * Generate realistic binary feed data
     */
    [[nodiscard]] std::vector<std::uint8_t> generateFeed();

    /**
     * Generate test feed with errors
     */
    [[nodiscard]] std::vector<std::uint8_t> generateTestFeed();

    /**
     * Generate real binary message data
     */
    [[nodiscard]] std::vector<std::uint8_t> generateBinaryFeed();

    /**
     * Get current configuration
     */
    [[nodiscard]] const Config& getConfig() const noexcept { return config_; }

private:
    Config config_;
    mutable std::mt19937 rng_;
    
    [[nodiscard]] TradeMessage generateMessage(std::uint32_t sequenceNumber);
    [[nodiscard]] std::vector<std::uint8_t> serializeMessage(const TradeMessage& message);
    [[nodiscard]] std::uint64_t generateTimestamp() const;
    [[nodiscard]] std::uint32_t generatePrice() const;
    [[nodiscard]] std::uint32_t generateQuantity() const;
    [[nodiscard]] std::uint32_t selectSymbolToken() const;
    [[nodiscard]] TradeSide generateTradeSide() const;
    void writeUint32(std::vector<std::uint8_t>& buffer, std::size_t offset, std::uint32_t value);
    void writeUint64(std::vector<std::uint8_t>& buffer, std::size_t offset, std::uint64_t value);
    [[nodiscard]] std::uint32_t calculateCRC32(const std::uint8_t* data, std::size_t size) const;
};

} // namespace nse::mtbt