#include "OBAnalizer.h"
void IncrementalData::add_row(Timestamp local_timestamp, Timestamp exchange_timestamp, Price price, Amount amount, char side, bool snapshot)
{
    //if ((m_local_timestamp.size()>0) && (local_timestamp<m_local_timestamp[m_local_timestamp.size()-1]))
    //    throw std::invalid_argument( "local_timestmaps are not sorted");
    m_local_timestamp.push_back(local_timestamp);
    m_exchange_timestamp.push_back(exchange_timestamp);
    m_price.push_back(price);
    m_amount.push_back(amount);
    m_side.push_back(side);
    m_snapshot.push_back(snapshot);
    ++m_size;
};

void IncrementalData::delete_last_row()
{
    m_local_timestamp.pop_back();
    m_exchange_timestamp.pop_back();
    m_price.pop_back();
    m_amount.pop_back();
    m_side.pop_back();
    m_snapshot.pop_back();
    --m_size;
};

bool L3OrderBookSnapshot::get_order_bucket_by_level(Price_Level level, char side, OrderBucket* result)
{
    if (side==BID)
    {
        if (level > m_bidside_size-1)
            return false;
        int l = 0;
        for(auto it = m_bidside.begin(); it != m_bidside.end(); it++)
        {
            if (l==level)
            {
                *result = (*it);
                return true;
            };
            ++l;
        };
    };   
    if (side==ASK)
    {
        if (level > m_askside_size-1)
            return false;
        int l = 0;
        for(auto it = m_askside.begin(); it != m_askside.end(); it++)
        {
            if (l==level)
            {
                *result = (*it);
                return true;
            };
            ++l;
        };
    };
    return false;
};

bool L3OrderBookSnapshot::new_order(Timestamp local_timestamp, Timestamp exchange_timestamp, Price price, Amount amount, char side, StringID orderID)
{
    m_local_timestamp = local_timestamp;
    m_exchange_timestmap = exchange_timestamp;
    if (side==BID)
    {
        if ((m_bidside_map.find(orderID)!=m_bidside_map.end()) || (m_askside_map.find(orderID)!=m_askside_map.end())) return false;
        if (m_bidside.begin()==m_bidside.end())
        {
            m_bidside_size++;
            auto it = m_bidside.begin();
            m_bidside.insert(it, OrderBucket(price));
            it--;
            it->orders.push_back(Order(price, amount, orderID));
            m_bidside_map.insert(std::pair<StringID, OrderBucket&>(orderID, *it));
            return true;
        }
        for(auto it = m_bidside.begin(); it != m_bidside.end(); it++)
        {

            if (it->price > price)
                continue;
            if (it->price == price)
            {
                it->orders.push_back(Order(price, amount, orderID));
                m_bidside_map.insert(std::pair<StringID, OrderBucket&>(orderID, *it));
                return true;
            };
            if (it->price < price)
            {
                m_bidside_size++;
                m_bidside.insert(it, OrderBucket(price));
                it--;
                it->orders.push_back(Order(price, amount, orderID));
                m_bidside_map.insert(std::pair<StringID, OrderBucket&>(orderID, *it));
                return true;
            };
        };
        m_bidside_size++;
        auto it = m_bidside.end();
        m_bidside.insert(it, OrderBucket(price));
        it--;
        it->orders.push_back(Order(price, amount, orderID));
        m_bidside_map.insert(std::pair<StringID, OrderBucket&>(orderID, *it));
        return true;
    };
    if (side==ASK)
    {
        if ((m_bidside_map.find(orderID)!=m_bidside_map.end()) || (m_askside_map.find(orderID)!=m_askside_map.end())) return false;
        if (m_askside.begin()==m_askside.end())
        {
            m_askside_size++;
            auto it = m_askside.begin();
            m_askside.insert(it, OrderBucket(price));
            it--;
            it->orders.push_back(Order(price, amount, orderID));
            m_askside_map.insert(std::pair<StringID, OrderBucket&>(orderID, *it));
            return true;
        }
        for(auto it = m_askside.begin(); it != m_askside.end(); it++)
        {

            if (it->price < price)
                continue;
            if (it->price == price)
            {
                it->orders.push_back(Order(price, amount, orderID));
                m_askside_map.insert(std::pair<StringID, OrderBucket&>(orderID, *it));
                return true;
            };
            if (it->price > price)
            {
                m_askside_size++;
                m_askside.insert(it, OrderBucket(price));
                it--;
                it->orders.push_back(Order(price, amount, orderID));
                m_askside_map.insert(std::pair<StringID, OrderBucket&>(orderID, *it));
                return true;
            };
        };
        m_askside_size++;
        auto it = m_askside.end();
        m_askside.insert(it, OrderBucket(price));
        it--;
        it->orders.push_back(Order(price, amount, orderID));
        m_askside_map.insert(std::pair<StringID, OrderBucket&>(orderID, *it));
        return true;
    };
};

bool L3OrderBookSnapshot::cancel_order(StringID orderID)
{
    auto result = m_bidside_map.find(orderID);
    if (result != m_bidside_map.end())
        for (auto it = result->second.orders.begin(); it != result->second.orders.end(); it++)
            if (it->orderID == orderID)
            {
                result->second.orders.erase(it);
                if (result->second.orders.empty())
                    for(auto to_delete = m_bidside.begin(); to_delete!=m_bidside.end(); to_delete++)
                        if (to_delete->price == result->second.price)
                        {
                            m_bidside.erase(to_delete);
                            m_bidside_map.erase(result);
                            return true;
                        };
                m_bidside_map.erase(result);
                return true;
            };
    
    result = m_askside_map.find(orderID);
    if (result != m_askside_map.end())
        for (auto it = result->second.orders.begin(); it != result->second.orders.end(); it++)
           if (it->orderID == orderID)
            {
                result->second.orders.erase(it);
                if (result->second.orders.empty())
                    for(auto to_delete = m_askside.begin(); to_delete!=m_askside.end(); to_delete++)
                        if (to_delete->price == result->second.price)
                        {
                            m_askside.erase(to_delete);
                            m_askside_map.erase(result);
                            return true;
                        };
                m_askside_map.erase(result);
                return true;
            };
    //if ()*/
    return false;
};

bool L3OrderBookSnapshot::change_order(Timestamp local_timestamp, Timestamp exchange_timestamp, Price price, Amount amount, char side, StringID orderID)
{
    //тут может быть проблема если удастся снять ордер, а поставить новый не получится
    //но ошибка при постановке на данный момент есть только если orderID дублируется,
    //так что теоретически такая проблема исключена
    return cancel_order(orderID) && new_order(local_timestamp, exchange_timestamp, price, amount, side, orderID);
};

void OrderBookSnapshot::insert_update(Timestamp local_timestamp, Timestamp exchange_timestamp, Price price, Amount amount, char side, bool is_snapshot)
{
    m_local_timestamp = local_timestamp;
    m_exchange_timestmap = exchange_timestamp;
    ++m_incremental_index;
    switch(side)
    {
        case BID:insert_into_bid(price, amount, is_snapshot); break;
        case ASK:insert_into_ask(price, amount, is_snapshot); break;
        default: throw OBException("Update side is unknown", m_local_timestamp, m_exchange_timestmap,
                                m_incremental_index, price, amount, Exception_Type::Unknown_update_side);
    };
};

void OrderBookSnapshot::insert_into_bid(Price price, Amount amount, bool is_snapshot)
{
    if(amount == 0)
    {
        auto prev = m_bidside.begin();
        for (auto it = m_bidside.begin(); it != m_bidside.end(); ++it)
        {
            if(it->first == price)
            {
                --m_bidside_size;
                if(it == m_bidside.begin())
                    m_bidside.pop_front();
                else
                    m_bidside.erase_after(prev);
                return;
            }
            else if((prev->first > price) && (it->first < price) && (!is_snapshot))
                throw OBException("Nonexistent level has been deleted", m_local_timestamp, m_exchange_timestmap,
                                m_incremental_index, price, amount, Exception_Type::Nonexistent_level);
            prev = it;                
        };
        return;
    };

    if((m_bidside.empty())||(m_bidside.begin()->first < price))
    {
        ++m_bidside_size;
        m_bidside.push_front(pair<Price, Amount>(price,amount));
        return;
    };
    auto it = m_bidside.begin();
    auto prev = m_bidside.begin();
    for (; it != m_bidside.end(); ++it)
    {
        if (it->first<price)
        {
            ++m_bidside_size;
            m_bidside.insert_after(prev, pair<Price, Amount>(price,amount));
            return;
        }
        else if(it->first == price)
        {
            *it = pair<Price, Amount>(price,amount);
            return;
        };
        prev = it;
    };
    if (it==m_bidside.end())
    {
        ++m_bidside_size;
        m_bidside.insert_after(prev, pair<Price, Amount>(price,amount)); 
    };
};

void OrderBookSnapshot::insert_into_ask(Price price, Amount amount, bool is_snapshot)
{
    if(amount == 0)
    {
        auto prev = m_askside.begin();
        for (auto it = m_askside.begin(); it != m_askside.end(); ++it)
        {
            if(it->first == price)
            {
                --m_askside_size;
                if(it == m_askside.begin())
                    m_askside.pop_front();
                else
                    m_askside.erase_after(prev);
                return;
            }
            else if((prev->first < price) && (it->first > price) && (!is_snapshot))
                throw OBException("Nonexistent level has been deleted", m_local_timestamp, m_exchange_timestmap,
                                m_incremental_index, price, amount, Exception_Type::Nonexistent_level);
            prev = it;                
        };
        return;
    };

    if((m_askside.empty())||(m_askside.begin()->first > price))
    {
        ++m_askside_size;
        m_askside.push_front(pair<Price, Amount>(price,amount));
        return;
    };
    auto it = m_askside.begin();
    auto prev = m_askside.begin();
    for (; it != m_askside.end(); ++it)
    {
        if (it->first>price)
        {
            ++m_askside_size;
            m_askside.insert_after(prev, pair<Price, Amount>(price,amount));
            return;
        }
        else if(it->first == price)
        {
            *it = pair<Price, Amount>(price,amount);
            return;
        };
        prev = it;
    };
    if (it==m_askside.end()) 
    {
        ++m_askside_size;
        m_askside.insert_after(prev, pair<Price, Amount>(price,amount)); 
    };
};

const pair<Price, Amount>& OrderBookSnapshot::get_bid_on_level(Price_Level level) const
{
    Price_Level ind = 0;
    auto iter = m_bidside.begin();
    for(; iter!=m_bidside.end() && ind!=level; ++iter) ++ind;
    if (ind == level && m_bidside_size!=0) return *iter;
    throw std::logic_error("Propogated level doesn't exist");
};

const pair<Price, Amount>& OrderBookSnapshot::get_ask_on_level(Price_Level level) const
{
    Price_Level ind = 0;
    auto iter = m_askside.begin();
    for(; iter!=m_askside.end() && ind!=level; ++iter) ++ind;
    if (ind == level && m_askside_size!=0) return *iter;
    throw std::logic_error("Propogated level doesn't exist");
};

void Worker::run()
{
    while (m_incremental_index < m_incremental_data.size())
    {
        m_step_forward();
    };
};

void Worker::m_step_forward()
{
    Timestamp current_ts = m_incremental_data.local_timestamp(m_incremental_index);
    m_order_book.insert_update(m_incremental_data.local_timestamp(m_incremental_index), 
                                m_incremental_data.exchange_timestamp(m_incremental_index), 
                                m_incremental_data.price(m_incremental_index), 
                                m_incremental_data.amount(m_incremental_index), 
                                m_incremental_data.side(m_incremental_index),
                                m_incremental_data.is_snapshot(m_incremental_index));
    ++m_incremental_index; 
    for (; (m_incremental_index < m_incremental_data.size()) && (m_incremental_data.local_timestamp(m_incremental_index-1) == m_incremental_data.local_timestamp(m_incremental_index)); ++m_incremental_index)
    {
        m_order_book.insert_update(m_incremental_data.local_timestamp(m_incremental_index), 
                                    m_incremental_data.exchange_timestamp(m_incremental_index), 
                                    m_incremental_data.price(m_incremental_index), 
                                    m_incremental_data.amount(m_incremental_index), 
                                    m_incremental_data.side(m_incremental_index),
                                    m_incremental_data.is_snapshot(m_incremental_index));
    };
    on_new_step();
};