#include "MessageTypes.h"

namespace nse::mtbt {

// Real NSE symbol token mappings (actual NSE tokens)
const std::unordered_map<std::uint32_t, std::string> SymbolRegistry::tokenToSymbol = {
    {3045, "SBIN"},        // State Bank of India
    {1270, "RELIANCE"},    // Reliance Industries
    {11536, "TCS"},        // Tata Consultancy Services
    {2885, "INFY"},        // Infosys Limited
    {1594, "HDFCBANK"},    // HDFC Bank
    {4963, "ICICIBANK"},   // ICICI Bank
    {8479, "BHARTIARTL"},  // Bharti Airtel
    {6364, "KOTAKBANK"},   // Kotak Mahindra Bank
    {1922, "ITC"},         // ITC Limited
    {5258, "LT"},          // Larsen & Toubro
    {7229, "HCLTECH"},     // HCL Technologies
    {2031, "AXISBANK"},    // Axis Bank
    {4717, "WIPRO"},       // Wipro Limited
    {9559, "TITAN"},       // Titan Company
    {7512, "NESTLEIND"},   // Nestle India
    {2475, "MARUTI"},      // Maruti Suzuki
    {3499, "BAJFINANCE"},  // Bajaj Finance
    {4668, "TECHM"},       // Tech Mahindra
    {1348, "SUNPHARMA"},   // Sun Pharmaceutical
    {5900, "ULTRACEMCO"},  // UltraTech Cement
    {14299, "ADANIPORTS"}, // Adani Ports
    {13538, "POWERGRID"},  // Power Grid Corporation
    {881, "HINDUNILVR"},   // Hindustan Unilever
    {694, "COALINDIA"},    // Coal India
    {16669, "ONGC"},       // Oil & Natural Gas Corporation
    {3063, "NTPC"},        // NTPC Limited
    {4143, "DRREDDY"},     // Dr. Reddy's Laboratories
    {225, "JSWSTEEL"},     // JSW Steel
    {1232, "BRITANNIA"},   // Britannia Industries
    {15083, "GRASIM"}      // Grasim Industries
};

} // namespace nse::mtbt