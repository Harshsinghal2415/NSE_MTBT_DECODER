#include "Utils.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nse::mtbt::utils {

std::string MessageFormatter::formatMessage(const TradeMessage& msg) {
    std::ostringstream oss;
    
    // Validate message first
    const auto validation = msg.validate();
    
    oss << std::left
        << "Seq: " << std::setw(6) << msg.sequenceNumber
        << " | Token: " << std::setw(6) << msg.symbolToken
        << " | Time: " << std::setw(12) << msg.timestamp
        << " | " << colors::GREEN << "₹" << std::setw(8) << std::fixed 
        << std::setprecision(2) << msg.getPriceInRupees() << colors::RESET
        << " | Qty: " << std::setw(6) << msg.quantity
        << " | " << colors::CYAN << formatTradeSide(msg.side) << colors::RESET;
    
    if (validation.isValid) {
        oss << " | " << colors::GREEN << "✓ VALID" << colors::RESET;
    } else {
        oss << " | " << colors::RED << "✗ INVALID";
        if (validation.errorMessage) {
            oss << " (" << *validation.errorMessage << ")";
        }
        oss << colors::RESET;
    }
    
    return oss.str();
}

std::string MessageFormatter::formatPrice(std::uint32_t priceInPaisa) {
    std::ostringstream oss;
    oss << "₹" << std::fixed << std::setprecision(2) << (priceInPaisa / 100.0);
    return oss.str();
}

std::optional<std::string> MessageFormatter::formatMessageAsCsv(const TradeMessage& msg) {
    if (const auto timeStr = msg.getFormattedTime(); timeStr) {
        std::ostringstream oss;
        oss << msg.sequenceNumber << ","
            << msg.symbolToken << ","
            << msg.timestamp << ","
            << std::fixed << std::setprecision(2) << msg.getPriceInRupees() << ","
            << msg.quantity << ","
            << formatTradeSide(msg.side) << ","
            << *timeStr << ","
            << (msg.isValid() ? "VALID" : "INVALID");
        return oss.str();
    }
    return std::nullopt;
}

std::string MessageFormatter::formatStats(const Decoder::DecodingStats& stats) {
    std::ostringstream oss;
    
    oss << colors::BOLD << colors::BLUE << "\n📊 Decoding Performance Statistics" 
        << colors::RESET << "\n"
        << colors::CYAN << "═══════════════════════════════════════════════════════════" 
        << colors::RESET << "\n";
    
    oss << colors::GREEN << "✅ Successfully decoded: " << colors::RESET << stats.decodedMessages << " messages\n";
    oss << colors::BLUE << "⚡ Processing speed:     " << colors::RESET << stats.processingSpeed << " msg/sec\n";
    oss << colors::YELLOW << "⏱️  Total time:          " << colors::RESET << stats.totalTimeUs << " μs\n";
    oss << colors::MAGENTA << "📦 Data processed:       " << colors::RESET << stats.bytesProcessed << " bytes\n";
    oss << colors::CYAN << "🔍 Valid messages:       " << colors::RESET << stats.validMessages << "/" << stats.decodedMessages << "\n";
    
    if (stats.errorCount > 0) {
        oss << colors::RED << "❌ Processing errors:   " << colors::RESET << stats.errorCount << "\n";
    }
    
    if (stats.truncatedBytes > 0) {
        oss << colors::YELLOW << "⚠️  Truncated bytes:    " << colors::RESET << stats.truncatedBytes << "\n";
    }
    
    return oss.str();
}

} // namespace nse::mtbt::utils