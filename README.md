## ITCH Parser & Data Handler

- Program to parse NASDAQ ITCH data, maintain an order book and explore the provided data.
- Testing done with 30/01/2019 data from [NASDAQ](https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH) with message format following the [NASDAQ TotalView-ITCH 5.0](https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf)
  - Data is listed as 1/31/2019, 1:18 AM

### MVP Targets

1. [x] Read and Decode Data
2. [x] Consume Order Book ITCH Messages (1.3.X & 1.4.X)
3. [x] Store Messages in DB
   1. [x] Setup Clickhouse locally
   2. [x] Save snapshots of order books in DB
4. [ ] Create Local Frontend
   1. [ ] Choose front-end
   2. [ ] Implement Simple Version to sanity check DB
   3. [ ] Order Book visualisation for each stock.
5. [ ] Cloud or Self-Host website to show front-end.
   1. [ ] Cloud vs Self-Host.
   2. [ ] Store DB.
   3. [ ] Connect Front-end.

### Could haves
1. [ ] Consume Other ITCH Messages
   1. [ ] Stock Info: 1.2.X
   2. [ ] Other...  : 1.5+ (For volume data)
2. [ ] Tickers for stocks.
   1. [ ] Different time periods. (1m, 5m, 10m)
   2. [ ] OCHL
3. [ ] Multiple Files (Different days);
4. [ ] Paralise
   1. [ ] Move Clickhouse Logging to own thread.
   2. [ ] Allow for processing of multiple files at once.

### What is ITCH

"ITCH is the revoluationary Nasdaq outbound portocol"<sup>[1](https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf)</sup> used when broadcasting events, trade, and other stock and exchange information. It specificies the different message types that can be broadcast and their structure. These messages are a big endian binary stream, requiring recipiants to identify and decode message in order to maintain their own records for items like order books, or displaying volume or other market statistics.

While the actuall feed uses a MoldUDP64 message format the testing data, mentioned above, uses the SoupBinTCP protocol option.

Put simply, ITCH is a binary data stream that can be used by downstream consumers to track any NASDAQ listed equities with their full order depth.
