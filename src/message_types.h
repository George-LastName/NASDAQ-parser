#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

struct [[gnu::packed]] ITCH_Header {
    char type;
    uint16_t locate;
    uint16_t track_num;
    uint8_t timestamp[6];

    uint64_t get_timestamp() const{
        uint64_t ti_st = 0;
        for(uint8_t t : timestamp){
            ti_st = (ti_st << 8) | t;
        }
        return ti_st;
    }
};

struct [[gnu::packed]] Sys_Event {
    ITCH_Header header;
    char event_code;
};

struct [[gnu::packed]] Stock_Dir {
    ITCH_Header header;
    char stock[8];
    char market_cat;
    char fin_stat_ind;
    uint32_t rnd_lot_size;
    char rnd_lots_only;
    char issue_class;
    char issue_sub[2];
    char auth;
    char srt_thr_ind;
    char IPO_flg;
    char LULD_ref;
    char ETP_flg;
    uint32_t ETP_lev;
    char inv_ind;
};

struct [[gnu::packed]] Stock_Trading_Action {
    ITCH_Header header;
    char stock[8];
    char trading_state; // 'H'=Halted, 'P'=Paused, 'Q'=Quotation, 'T'=Trading
    char reserved;
    char reason[4];
};

struct [[gnu::packed]] Reg_Sho_Restriction {
    ITCH_Header header;
    char stock[8];
    char reg_sho_action; // '0'=No price test, '1'=In effect, '2'=Remains in effect
};

struct [[gnu::packed]] Market_Participant_Position {
    ITCH_Header header;
    char mpid[4];
    char stock[8];
    char primary_market_maker;
    char market_maker_mode;
    char market_participant_state;
};

struct [[gnu::packed]] MWCB_Decline_Level {
    ITCH_Header header;
    uint64_t level_1;
    uint64_t level_2;
    uint64_t level_3;
};

struct [[gnu::packed]] MWCB_Status {
    ITCH_Header header;
    char breached_level;
};

struct [[gnu::packed]] Quoting_Period_Update {
    ITCH_Header header;
    char stock[8];
    uint32_t IPO_quot_release_time;
    char IPO_quot_release_qualifier;
    uint32_t IPO_price;
};

struct [[gnu::packed]] LULD_Auction_Collar {
    ITCH_Header header;
    char stock[8];
    uint32_t auction_collar_ref_price;
    uint32_t upper_auction_collar_price;
    uint32_t lower_auction_collar_price;
    uint32_t auction_collar_ext;
};

struct [[gnu::packed]] Operational_Halt {
    ITCH_Header header;
    char stock[8];
    char market_code;
    char halt_action;
};

struct [[gnu::packed]] Add_Order_No_MPID {
    ITCH_Header header;
    uint64_t order_reference_number;
    char buy_sell_indicator; // 'B'=Buy, 'S'=Sell
    uint32_t shares;
    char stock[8];
    uint32_t price; // Price integer (divide by 10,000 for decimal)
};

struct [[gnu::packed]] Add_Order_MPID {
    ITCH_Header header;
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price; // Price integer (divide by 10,000 for decimal)
    char attribution[4];
};

struct [[gnu::packed]] Order_Executed {
    ITCH_Header header;
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
};

struct [[gnu::packed]] Order_Executed_With_Price {
    ITCH_Header header;
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
    char printable; // 'Y' or 'N'
    uint32_t execution_price;
};

struct [[gnu::packed]] Order_Cancel {
    ITCH_Header header;
    uint64_t order_reference_number;
    uint32_t canceled_shares;
};

struct [[gnu::packed]] Order_Delete {
    ITCH_Header header;
    uint64_t order_reference_number;
};

struct [[gnu::packed]] Order_Replace {
    ITCH_Header header;
    uint64_t original_order_reference_number;
    uint64_t new_order_reference_number;
    uint32_t shares;
    uint32_t price;
};

struct [[gnu::packed]] Trade_Non_Cross {
    ITCH_Header header;
    uint64_t order_reference_number; // Always 0 for this message
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    uint64_t match_number;
};

struct [[gnu::packed]] Cross_Trade {
    ITCH_Header header;
    uint64_t shares;
    char stock[8];
    uint32_t cross_price;
    uint64_t match_number;
    char cross_type; // 'O'=Open, 'C'=Close, 'H'=Halt/IPO, 'I'=Intraday
};

struct [[gnu::packed]] Broken_Trade {
    ITCH_Header header;
    uint64_t match_number;
};

struct [[gnu::packed]] NOII {
    ITCH_Header header;
    uint64_t paired_shares;
    uint64_t imbalance_shares;
    char imbalance_direction; // 'B', 'S', 'N', 'O'
    char stock[8];
    uint32_t far_price;
    uint32_t near_price;
    uint32_t current_reference_price;
    char cross_type;
    char price_variation_indicator;
};

struct [[gnu::packed]] RPII { // Retail Price Improvement Indicator
    ITCH_Header header;
    char stock[8];
    char interest_flag; // 'B', 'S', 'A', 'N'
};

struct [[gnu::packed]] DLCR_Price_Discovery {
    ITCH_Header header;
    char stock[8];
    char open_eligibility_status;
    uint32_t min_allow_price;
    uint32_t max_allow_price;
    uint32_t near_exe_price;
    uint64_t near_exe_time;
    uint32_t lower_price_range_collar;
    uint32_t upper_price_range_collar;
};

#endif
