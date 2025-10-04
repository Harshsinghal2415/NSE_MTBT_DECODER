#include "FeedSimulator.h"
#include "Decoder.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <iomanip>
#include <optional>

namespace nse::mtbt::app {

/**
 * Modern C++17 application configuration
 */
struct ApplicationConfig {
    bool writeCsv{false};
    bool testErrors{false};
    bool enableColors{true};
    bool showStats{true};
    std::size_t messageCount{1000};
    std::string outputPath{"decoded_output.csv"};
    std::optional<std::uint32_t> randomSeed{std::nullopt};
    ValidationLevel validationLevel{ValidationLevel::STRICT};
    
    [[nodiscard]] bool isValid() const noexcept {
        return messageCount > 0 && messageCount <= 1'000'000 && !outputPath.empty();
    }
};

/**
 * Print usage information
 */
void printUsage(const std::string& programName) noexcept {
    using namespace nse::mtbt::utils;
    
    std::cout << colors::BOLD << colors::CYAN 
              << "🚀 NSE MTBT Feed Decoder (Modern C++17)\n" << colors::RESET
              << colors::GREEN << "Industry-ready high-performance market data decoder\n\n" << colors::RESET
              << colors::BOLD << "Usage: " << colors::RESET << programName << " [options]\n\n"
              << colors::BOLD << "Options:\n" << colors::RESET
              << "  " << colors::YELLOW << "--csv" << colors::RESET 
              << "              Export decoded messages to CSV file\n"
              << "  " << colors::YELLOW << "--count N" << colors::RESET 
              << "          Generate N messages (default: 1000, max: 1M)\n"
              << "  " << colors::YELLOW << "--output PATH" << colors::RESET 
              << "      Specify output CSV file path\n"
              << "  " << colors::YELLOW << "--test-errors" << colors::RESET 
              << "     Include malformed messages for testing\n"
              << "  " << colors::YELLOW << "--seed N" << colors::RESET 
              << "          Set random seed for reproducibility\n"
              << "  " << colors::YELLOW << "--help" << colors::RESET 
              << "            Show this help message\n\n"
              << colors::BOLD << "Examples:\n" << colors::RESET
              << "  " << programName << " --count 10000 --csv\n"
              << "  " << programName << " --test-errors --seed 42\n"
              << "  " << programName << " --count 100 --output trades.csv\n";
}

/**
 * Modern argument parser
 */
[[nodiscard]] std::optional<ApplicationConfig> parseArguments(int argc, char* argv[]) noexcept {
    ApplicationConfig config{};
    
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        
        if (arg == "--csv") {
            config.writeCsv = true;
        } else if (arg == "--test-errors") {
            config.testErrors = true;
        } else if (arg == "--help") {
            return std::nullopt; // Signal to show help
        } else if (arg == "--count" && i + 1 < argc) {
            try {
                const auto count = std::stoull(argv[++i]);
                if (count == 0 || count > 1'000'000) {
                    std::cerr << "❌ Error: Message count must be 1-1,000,000\n";
                    return std::nullopt;
                }
                config.messageCount = static_cast<std::size_t>(count);
            } catch (const std::exception&) {
                std::cerr << "❌ Error: Invalid count value\n";
                return std::nullopt;
            }
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputPath = argv[++i];
            config.writeCsv = true;
        } else if (arg == "--seed" && i + 1 < argc) {
            try {
                config.randomSeed = static_cast<std::uint32_t>(std::stoul(argv[++i]));
            } catch (const std::exception&) {
                std::cerr << "❌ Error: Invalid seed value\n";
                return std::nullopt;
            }
        } else {
            std::cerr << "❌ Error: Unknown argument: " << arg << "\n";
            return std::nullopt;
        }
    }
    
    return config.isValid() ? std::optional{config} : std::nullopt;
}

/**
 * Write CSV output
 */
[[nodiscard]] bool writeCsvOutput(const std::vector<TradeMessage>& messages, 
                                 const std::string& outputPath) noexcept {
    using namespace nse::mtbt::utils;
    
    try {
        std::ofstream csvFile{outputPath};
        if (!csvFile.is_open()) {
            return false;
        }
        
        // Write header
        csvFile << "Sequence,Symbol,Timestamp,Price(INR),Quantity,Side,Time,Status\n";
        
        // Write all messages
        for (const auto& msg : messages) {
            if (auto csvLine = MessageFormatter::formatMessageAsCsv(msg); csvLine) {
                csvFile << *csvLine << "\n";
            }
        }
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace nse::mtbt::app

int main(int argc, char* argv[]) {
    using namespace nse::mtbt;
    using namespace nse::mtbt::utils;
    using namespace nse::mtbt::app;
    
    // Parse command line arguments
    const auto configOpt = parseArguments(argc, argv);
    
    if (!configOpt) {
        printUsage(argv[0]);
        return configOpt.has_value() ? 1 : 0;
    }
    
    const auto& config = *configOpt;
    
    std::cout << colors::BOLD << colors::CYAN 
              << "🚀 NSE MTBT Feed Decoder (Modern C++17)" << colors::RESET << "\n"
              << colors::GREEN << "═══════════════════════════════════════════════════════════" 
              << colors::RESET << "\n";
    
    std::cout << colors::BLUE << "📊 Processing " << config.messageCount << " messages";
    if (config.testErrors) {
        std::cout << " (including error simulation)";
    }
    if (config.randomSeed) {
        std::cout << " [seed: " << *config.randomSeed << "]";
    }
    std::cout << "...\n" << colors::RESET;
    
    // Configure and generate feed data
    FeedSimulator::Config simConfig{};
    simConfig.messageCount = config.messageCount;
    simConfig.seed = config.randomSeed.value_or(0);
    simConfig.malformedCount = config.testErrors ? config.messageCount / 20 : 0;
    simConfig.validationLevel = config.validationLevel;
    
    FeedSimulator simulator{simConfig};
    
    // Generate feed data with timing
    const PerformanceMonitor::Timer generationTimer{};
    const auto feedData = config.testErrors ? 
        simulator.generateTestFeed() : simulator.generateFeed();
    const auto generationTime = generationTimer.elapsedMicroseconds();
    
    std::cout << colors::GREEN << "✅ Generated " << feedData.size() 
              << " bytes of feed data in " << generationTime << " μs\n" << colors::RESET;
    
    // Decode the feed
    Decoder decoder{};
    decoder.setValidationLevel(config.validationLevel);
    
    const auto messages = decoder.decodeFeed(feedData);
    
    // Display results
    std::cout << colors::BOLD << colors::MAGENTA 
              << "\n📈 Decoded Messages (" << messages.size() << " valid)" 
              << colors::RESET << "\n"
              << colors::CYAN << "═══════════════════════════════════════════════════════════" 
              << colors::RESET << "\n";
    
    // Display sample messages (first 10)
    const auto samplesToShow = std::min(messages.size(), std::size_t{10});
    for (std::size_t i = 0; i < samplesToShow; ++i) {
        std::cout << MessageFormatter::formatMessage(messages[i]) << "\n";
    }
    
    if (messages.size() > samplesToShow) {
        std::cout << colors::YELLOW << "... (" << (messages.size() - samplesToShow) 
                  << " more messages)\n" << colors::RESET;
    }
    
    // Handle CSV output
    if (config.writeCsv) {
        if (writeCsvOutput(messages, config.outputPath)) {
            std::cout << colors::GREEN << "\n💾 Successfully saved " << messages.size() 
                      << " messages to " << config.outputPath << "\n" << colors::RESET;
        } else {
            std::cerr << colors::RED << "\n❌ Failed to write CSV output to " 
                      << config.outputPath << "\n" << colors::RESET;
        }
    }
    
    // Display comprehensive statistics
    if (config.showStats) {
        std::cout << MessageFormatter::formatStats(decoder.getStats());
    }
    
    std::cout << colors::BOLD << colors::GREEN 
              << "\n🎉 Processing completed successfully!" << colors::RESET << "\n";
    
    return 0;
}