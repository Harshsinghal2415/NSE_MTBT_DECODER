#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <string_view>

namespace nse::mtbt {

/**
 * Modern C++17 Trade Side enumeration
 */
enum class TradeSide : std::uint8_t {
    BUY = 0,
    SELL = 1
};

/**
 * Validation levels for message processing
 */
enum class ValidationLevel : std::uint8_t {
    LENIENT = 0,
    STRICT = 1,
    CHECKSUM = 2
};

/**
 * Modern C++17 validation result with optional error message
 */
struct ValidationResult {
    bool isValid;
    std::optional<std::string> errorMessage;
    
    // C++17 structured binding support
    operator bool() const noexcept { return isValid; }
};

/**
 * NSE MTBT (Market Trade by Trade) message structure
 * Follows NSE specifications for trade data dissemination
 */
struct TradeMessage {
    std::uint32_t sequenceNumber{0};  // Message sequence number
    std::uint32_t symbolToken{0};     // NSE instrument token
    std::uint64_t timestamp{0};       // Trade timestamp (nanoseconds since epoch)
    std::uint32_t priceInPaisa{0};    // Trade price in paisa (1/100 rupee)
    std::uint32_t quantity{0};        // Trade quantity
    TradeSide side{TradeSide::BUY};   // Trade side (BUY/SELL)
    
    TradeMessage() = default;
    
    TradeMessage(std::uint32_t seq, std::uint32_t token, std::uint64_t ts,
                std::uint32_t price, std::uint32_t qty, TradeSide tradeSide)
        : sequenceNumber(seq), symbolToken(token), timestamp(ts),
          priceInPaisa(price), quantity(qty), side(tradeSide) {}
    
    /**
     * Get price in rupees with proper decimal conversion
     */
    [[nodiscard]] double getPriceInRupees() const noexcept {
        return static_cast<double>(priceInPaisa) / 100.0;
    }
    
    /**
     * Modern C++17 validation with structured binding support
     */
    [[nodiscard]] ValidationResult validate() const noexcept {
        if (sequenceNumber == 0) {
            return {false, "Invalid sequence number"};
        }
        if (symbolToken == 0) {
            return {false, "Invalid symbol token"};
        }
        if (timestamp == 0) {
            return {false, "Invalid timestamp"};
        }
        // NSE price validation (₹0.05 to ₹1,00,000)
        if (priceInPaisa < 5) {
            return {false, "Price below minimum tick size"};
        }
        if (quantity == 0) {
            return {false, "Invalid quantity"};
        }
        if (side != TradeSide::BUY && side != TradeSide::SELL) {
            return {false, "Invalid side"};
        }
        
        // NSE upper circuit limit
        if (priceInPaisa > 10'000'000) { // ₹1,00,000
            return {false, "Price exceeds NSE upper circuit"};
        }
        if (quantity > 10'000'000) { // NSE maximum order quantity
            return {false, "Quantity exceeds NSE limit"};
        }
        
        return {true, std::nullopt};
    }
    
    /**
     * Check if message is valid (simple boolean check)
     */
    [[nodiscard]] bool isValid() const noexcept {
        return validate().isValid;
    }
    
    /**
     * Get formatted timestamp string
     */
    [[nodiscard]] std::optional<std::string> getFormattedTime() const noexcept {
        if (timestamp == 0) return std::nullopt;
        
        // Simplified time formatting
        return "T" + std::to_string(timestamp);
    }
};

/**
 * Modern C++17 helper functions
 */
[[nodiscard]] inline std::string_view formatTradeSide(TradeSide side) noexcept {
    return (side == TradeSide::BUY) ? "BUY" : "SELL";
}

} // namespace nse::mtbt