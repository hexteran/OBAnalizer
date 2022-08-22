#define EXPORT __attribute__((visibility("default")))

#include <iostream>
#include <vector>
#include <forward_list>
#include <stdexcept>
#include <limits>

using namespace std;

#define UNDEFINED 0
#define BID 1
#define ASK 2 

typedef long Timestamp;
typedef float Price;
typedef float Amount;
typedef char Side;
typedef int Price_Level;

enum Exception_Type{Nonexistent_level, Not_sorted_incrementals, Unknown_update_side};

class OBException : exception
{
private:
    Exception_Type m_type;
    Timestamp m_local_timestamp, m_exchange_timestamp;
    Price m_price;
    Amount m_amount;
    long m_incremental_index;
    string m_description; 
public:
    OBException(const string& description, Timestamp local_timestamp, 
                Timestamp exchange_timestamp, long incremental_index,
                Price price, Amount amount, Exception_Type type): 
                m_local_timestamp(local_timestamp), 
                m_exchange_timestamp(exchange_timestamp), m_incremental_index(incremental_index),
                m_price(price), m_amount(amount), m_type(type)
                {
                    m_description = description + 
                    "\nlocal_timestamp:" + to_string(local_timestamp) + 
                    "\nexchange_timestamp:" + to_string(exchange_timestamp) +
                    "\nincremental_index:" + to_string(incremental_index) +
                    "\nprice:" + to_string(price) +
                    "\namount:" + to_string(amount);
                };

    Exception_Type type() const {return m_type;};

    const string& what() {return m_description;};
};

struct Options
{
    bool ignore_nonexistent = false;
};

class EXPORT IncrementalData
/*
    This is the class to work with raw incremental data
*/
{
private:
    vector<Timestamp> m_local_timestamp;
    vector<Timestamp> m_exchange_timestamp;
    vector<Price> m_price;
    vector<Amount> m_amount;
    vector<char> m_side;
    vector<bool> m_snapshot;
    int m_size;
public:
    IncrementalData(Price price_step): m_size(0){};
    void add_row(Timestamp local_timestamp, Timestamp exchange_timestamp, Price price, Amount amount, char side, bool snapshot);
    void delete_last_row();
    Timestamp local_timestamp(int i) const { return m_local_timestamp[i];};
    Timestamp exchange_timestamp(int i) const { return m_exchange_timestamp[i];};
    Price price(int i) const { return m_price[i];};
    Amount amount(int i) const { return m_amount[i];};
    Side side(int i) const { return m_side[i];};
    bool is_snapshot(int i) const { return m_snapshot[i];};
    long size() const { return m_size;}; 
};

class EXPORT OrderBookSnapshot
/*
    Objects of this class store immidiate snapshots
*/
{
private:
    Price m_price_step;
    Price m_min_price;    
    Timestamp m_local_timestamp;
    Timestamp m_exchange_timestmap;
    forward_list<pair<Price,Amount>> m_bidside, m_askside;
    int m_bidside_size, m_askside_size, m_incremental_index;
    void insert_into_bid(Price price, Amount amount, bool is_snapshot);
    void insert_into_ask(Price price, Amount amount, bool is_snapshot);
    void move_bid_after_ask();
public:
    OrderBookSnapshot(): m_bidside_size(0), m_askside_size(0) {};
    Timestamp get_local_timestamp() const {return m_local_timestamp; };
    Timestamp get_exchange_timestamp() const {return m_exchange_timestmap; };
    int get_bid_size() const {return m_bidside_size;};
    int get_ask_size() const {return m_askside_size;};
    const pair<Price, Amount>& get_bid_on_level(Price_Level level) const;
    const pair<Price, Amount>& get_ask_on_level(Price_Level level) const;
    void insert_update(Timestamp local_timestamp, Timestamp exchange_timestamp, Price price, Amount amount, char side, bool is_snapshot);
};
class EXPORT Worker
/*
    This class manages user access to orderbook snapshots at any particular time
*/
{
protected:
    Options m_options;
    OrderBookSnapshot m_order_book;
    const IncrementalData& m_incremental_data;
    int m_incremental_index;
    void m_step_forward();
public:
    Worker(const IncrementalData& incremental_data): m_incremental_data(incremental_data), m_incremental_index(0) {};
    void run(); // user calls this method to launch the working loop
    void ignore_nonexistent(bool){ m_options.ignore_nonexistent = true; };
    virtual void on_new_step() = 0; 
};
