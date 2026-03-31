## ITCH Parser & Data Handler

- Program to parse NASDAQ ITCH data, maintain an order book and explore the provided data.
- Testing done with 30/01/2019 data from [NASDAQ](https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH) with message format following the [NASDAQ TotalView-ITCH 5.0](https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf)

### Goals

1. [x] Read and Decode Data
2. [ ] Consume Decoded ITCH Messages
   21. [ ] Order Book: 1.3.X and 1.4.X (WIP)
   22. [ ] Stock Info: 1.2.X
   23. [ ] Other...  : 1.5+
3. [ ] Control Speed, have "real time", 2x, 3x.
4. [ ] GUI(?) of order book, with price, depth and volume graphs
or
5. [ ] Dummy Server-Client server setup to mimic receiving from NASDAQ.
