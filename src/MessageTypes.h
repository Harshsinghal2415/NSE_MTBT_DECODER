#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace nse::mtbt {

/**
 * Real NSE symbol token mappings
 */
class SymbolRegistry {
public:
    static const std::unordered_map<std::uint32_t, std::string> tokenToSymbol;
    
    [[nodiscard]] static std::optional<std::string> getSymbol(std::uint32_t token) noexcept {
        auto it = tokenToSymbol.find(token);
        return (it != tokenToSymbol.end()) ? std::optional<std::string>{it->second} : std::nullopt;
    }
    
    [[nodiscard]] static std::string getSymbolOrToken(std::uint32_t token) noexcept {
        if (auto symbol = getSymbol(token)) {
            return *symbol;
        }
        return "TOKEN_" + std::to_string(token);
    }
};

/**
 * Binary pattern utilities for bit-level decoding
 */
class BinaryUtils {
public:
    /**
     * Convert value to binary string representation
     */
    template<typename T>
    [[nodiscard]] static std::string toBinary(T value) {
        std::string binary;
        const std::size_t bits = sizeof(T) * 8;
        binary.reserve(bits + (bits / 4 - 1)); // Space for separators
        
        for (std::size_t i = 0; i < bits; ++i) {
            if (i > 0 && i % 4 == 0) {
                binary += ' ';
            }
            binary += ((value >> (bits - 1 - i)) & 1) ? '1' : '0';
        }
        return binary;
    }
    
    /**
     * Convert raw bytes to hex string
     */
    [[nodiscard]] static std::string toHex(const std::uint8_t* data, std::size_t size) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < size; ++i) {
            oss << std::setw(2) << static_cast<unsigned>(data[i]);
            if (i < size - 1) oss << " ";
        }
        return oss.str();
    }
    
    /**
     * Extract bits from a specific range
     */
    template<typename T>
    [[nodiscard]] static T extractBits(T value, std::uint8_t startBit, std::uint8_t numBits) {
        const T mask = ((T(1) << numBits) - 1);
        return (value >> startBit) & mask;
    }
};

/**
 * NSE MTBT binary protocol constants
 */
struct ProtocolConstants {
    static constexpr std::size_t MESSAGE_SIZE = 40;  // NSE MTBT message size
    static constexpr std::size_t HEADER_SIZE = 8;    // Message header size
    static constexpr std::uint32_t MAGIC_BYTES = 0xDEADBEEF;  // Protocol magic
    
    // Field offsets in the binary message
    static constexpr std::size_t OFFSET_SEQUENCE = 0;
    static constexpr std::size_t OFFSET_SYMBOL_TOKEN = 4;
    static constexpr std::size_t OFFSET_TIMESTAMP = 8;
    static constexpr std::size_t OFFSET_PRICE = 16;
    static constexpr std::size_t OFFSET_QUANTITY = 20;
    static constexpr std::size_t OFFSET_SIDE = 24;
    static constexpr std::size_t OFFSET_CHECKSUM = 36;
};

/**
 * Trade Side enumeration
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
 * Validation result with optional error message
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
    std::uint32_t checksum{0};        // CRC32 checksum
    
    TradeMessage() = default;
    
    TradeMessage(std::uint32_t seq, std::uint32_t token, std::uint64_t ts,
                std::uint32_t price, std::uint32_t qty, TradeSide tradeSide, std::uint32_t crc = 0)
        : sequenceNumber(seq), symbolToken(token), timestamp(ts),
          priceInPaisa(price), quantity(qty), side(tradeSide), checksum(crc) {}
    
    /**
     * Get price in rupees with proper decimal conversion
     */
    [[nodiscard]] double getPriceInRupees() const noexcept {
        return static_cast<double>(priceInPaisa) / 100.0;
    }
    
    /**
     * Get symbol name from token
     */
    [[nodiscard]] std::string getSymbolName() const noexcept {
        return SymbolRegistry::getSymbolOrToken(symbolToken);
    }
    
    /**
     * Get binary representation of symbol token
     */
    [[nodiscard]] std::string getTokenBinary() const noexcept {
        return BinaryUtils::toBinary(symbolToken);
    }
    
    /**
     * Validation with structured binding support
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
    
    /**
     * Get detailed decoding information
     */
    [[nodiscard]] std::string getDecodingInfo() const noexcept {
        std::ostringstream oss;
        oss << "Token Binary: " << getTokenBinary() 
            << " → Token " << symbolToken 
            << " → " << getSymbolName();
        return oss.str();
    }
};

/**
 * Helper functions
 */
[[nodiscard]] inline std::string_view formatTradeSide(TradeSide side) noexcept {
    return (side == TradeSide::BUY) ? "BUY" : "SELL";
}

} // namespace nse::mtbt