## ITCH Parser & Data Handler

- Program to parse NASDAQ ITCH data, maintain an order book and explore the provided data.
- Testing done with 30/01/2019 data from [NASDAQ](https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH) with message format following the [NASDAQ TotalView-ITCH 5.0](https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf)
  - Data is listed as 1/31/2019, 1:18 AM

### Goals

1. [x] Read and Decode Data
2. [ ] Consume Decoded ITCH Messages
   1. [ ] Order Book: 1.3.X and 1.4.X (WIP)
   2. [ ] Stock Info: 1.2.X
   3. [ ] Other...  : 1.5+
3. [ ] Control Speed, have "real time", 2x, 3x.
4. [ ] GUI(?) of order book, with price, depth and volume graphs
or
5. [ ] Dummy Server-Client server setup to mimic receiving from NASDAQ.

### What is ITCH

"ITCH is the revoluationary Nasdaq outbound portocol"<sup>[1](https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf)</sup> used when broadcasting events, trade, and other stock and exchange information. It specificies the different message types that can be broadcast and their structure. These messages are a big endian binary stream, requiring recipiants to identify and decode message in order to maintain their own records for items like order books, or displaying volume or other market statistics.

While the actuall feed uses a MoldUDP64 message format the testing data, mentioned above, uses the SoupBinTCP protocol option.

Put simply, ITCH is a binary data stream that can be used by downstream consumers to track any NASDAQ listed equities with their full order depth.
