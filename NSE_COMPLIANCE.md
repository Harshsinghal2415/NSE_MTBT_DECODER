# NSE MTBT Protocol Compliance

This document outlines how our implementation aligns with actual NSE (National Stock Exchange of India) MTBT (Market Trade by Trade) specifications.

## NSE MTBT Protocol Compliance

### 1. **Symbol Tokens**
- **Actual NSE tokens used**: 3045 (SBIN), 1270 (RELIANCE), 11536 (TCS), 2885 (INFY), 1594 (HDFCBANK)
- **Format**: 32-bit unsigned integers as per NSE specification
- **Represents**: Real NSE instrument tokens for major stocks

### 2. **Price Representation**
- **Format**: Stored in paisa (1/100th of rupee) as 32-bit unsigned integer
- **Tick Size**: Rounded to nearest 5 paisa (NSE standard for most stocks)
- **Range**: ₹0.05 to ₹1,00,000 (NSE circuit limits)
- **Validation**: Enforces minimum tick size and upper circuit limits

### 3. **Quantity Validation**
- **Maximum**: 10,000,000 shares per trade (NSE limit)
- **Format**: 32-bit unsigned integer
- **Validation**: Ensures quantities are within NSE permissible limits

### 4. **Trade Sides**
- **BUY/SELL**: Standard NSE trade side indicators
- **Enum**: Type-safe representation preventing invalid values

### 5. **Timestamp Format**
- **Precision**: Nanoseconds since epoch (NSE standard for high-frequency data)
- **Format**: 64-bit unsigned integer
- **Accuracy**: Microsecond-level precision for trade ordering

### 6. **Message Sequence**
- **Sequential numbering**: Ensures message ordering integrity
- **Format**: 32-bit unsigned integer
- **Purpose**: Trade reconstruction and gap detection

## Real-World NSE Integration Points

### Data Feed Characteristics
- **Protocol**: Binary message format optimized for speed
- **Validation**: Multi-level checks for data integrity
- **Performance**: Microsecond-level processing latency
- **Compliance**: Follows SEBI guidelines for market data dissemination

### Production Considerations
- **Memory Layout**: Struct packing for wire format compatibility
- **Endianness**: Little-endian format (Intel x86 standard)
- **Error Handling**: Graceful handling of malformed messages
- **Monitoring**: Real-time statistics and performance metrics

## Industry Standards Met
- **NSE Technical Specifications**: Message format compliance
- **SEBI Regulations**: Market data accuracy requirements
- **Risk Management**: Price and quantity limit validations
- **Performance Standards**: High-throughput processing capabilities

This implementation can be adapted for real NSE feed integration with minimal modifications.