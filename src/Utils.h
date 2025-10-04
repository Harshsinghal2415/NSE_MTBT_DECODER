#pragma once

#include "MessageTypes.h"
#include "Decoder.h"
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace nse::mtbt::utils {

/**
 * Color codes for modern console output
 */
namespace colors {
    inline constexpr const char* RESET = "\033[0m";
    inline constexpr const char* BOLD = "\033[1m";
    inline constexpr const char* RED = "\033[31m";
    inline constexpr const char* GREEN = "\033[32m";
    inline constexpr const char* YELLOW = "\033[33m";
    inline constexpr const char* BLUE = "\033[34m";
    inline constexpr const char* MAGENTA = "\033[35m";
    inline constexpr const char* CYAN = "\033[36m";
}

/**
 * Message formatting utilities
 */
class MessageFormatter {
public:
    /**
     * Format a single trade message with colors
     */
    [[nodiscard]] static std::string formatMessage(const TradeMessage& msg);

    /**
     * Format message as CSV line
     */
    [[nodiscard]] static std::optional<std::string> formatMessageAsCsv(const TradeMessage& msg);

    /**
     * Format processing statistics
     */
    [[nodiscard]] static std::string formatStats(const Decoder::DecodingStats& stats);

    /**
     * Format price with currency symbol
     */
    [[nodiscard]] static std::string formatPrice(std::uint32_t priceInPaisa);
};

/**
 * Performance monitoring
 */
class PerformanceMonitor {
public:
    /**
     * Simple timer class for performance measurement
     */
    class Timer {
    public:
        Timer() noexcept : start_{std::chrono::high_resolution_clock::now()} {}
        
        [[nodiscard]] auto elapsed() const noexcept {
            return std::chrono::high_resolution_clock::now() - start_;
        }
        
        [[nodiscard]] std::uint64_t elapsedMicroseconds() const noexcept {
            const auto duration = elapsed();
            return static_cast<std::uint64_t>(
                std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
        }
        
    private:
        std::chrono::high_resolution_clock::time_point start_;
    };
};

} // namespace nse::mtbt::utils