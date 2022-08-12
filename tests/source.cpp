#include "../src/Serializer.hpp"
#include <string>

struct Strategy : public Worker
{
    vector<pair<Price, Amount>> Result, Result_ask;
    Strategy(const IncrementalData& incremental_data) : Worker(incremental_data){}
    virtual void on_new_step()
    {
        if (m_incremental_index == (m_incremental_data.size()-1))
        {
            for(int i = 0; i < 7; i++)
            {
                if (i<m_order_book.get_bid_size())
                {
                    std::cout<<m_order_book.get_bid_on_level(i).first<<" "<<m_order_book.get_bid_on_level(i).second<<std::endl;
                    Result.push_back(m_order_book.get_bid_on_level(i));
                };
                if (i<m_order_book.get_ask_size())
                {
                    std::cout<<m_order_book.get_ask_on_level(i).first<<" "<<m_order_book.get_ask_on_level(i).second<<std::endl;
                    Result_ask.push_back(m_order_book.get_ask_on_level(i));
                };
            };        
        };
    }; 
};

struct DeribitStrategy : public Worker
//this is the strategy to collect bbo on real data
{
    fstream output;
    DeribitStrategy(const IncrementalData& incremental_data) : Worker(incremental_data), 
                                                               output("/home/afaradzhov/OBAnalizer/tests/datasets/output.csv", ios::out)
    {
        output<<"local_timestamp,exchange_timestamp,bid_price,bid_amount,ask_price,ask_amount,spread\n";
    }
    virtual void on_new_step()
    {
        if (m_order_book.get_bid_size()!=0 && m_order_book.get_ask_size()!=0)
        {
            output<<m_order_book.get_local_timestamp()<<","<<m_order_book.get_exchange_timestamp()<<",";
            output<<m_order_book.get_bid_on_level(0).first<<","<<m_order_book.get_bid_on_level(0).second<<",";
            output<<m_order_book.get_ask_on_level(0).first<<","<<m_order_book.get_ask_on_level(0).second<<",";
            output<<m_order_book.get_ask_on_level(0).first-m_order_book.get_bid_on_level(0).first;
            output<<std::endl;
        };     
    };
}; 

int test_on_deribit()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_deribit.bin", ios::in | std::ios::binary);
    int i = 0;
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        ++i;
    };
    DeribitStrategy strat(incremental_data);
    input.close();
    strat.run();
    return 1;    
};

int insert_unique_new_undefined_side()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test4.bin", ios::in | std::ios::binary);
    int i = 0;
    while (i<50000 && !input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
        ++i;
    };
    Strategy strat(incremental_data);
    input.close();
    try{
        strat.run();
    }
    catch(std::logic_error& e)
    {
        std::cout<<e.what();
        if (string(e.what()) == "Unknown update side") return 0;
    };    
    throw std::logic_error("insert_unique_new_undefined_side_bid");
    return 1;
}

int delete_inexistent_level_ask()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test10.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };
    Strategy strat(incremental_data);
    input.close();
    try{
        strat.run();
    }
    catch(std::logic_error& e)
    {
        std::cout<<e.what();
        if (string(e.what()) == "Nonexistent level has been deleted") return 0;
    };    
    throw std::logic_error("delete_inexistent_level_ask");
    return 0;
}

int delete_inexistent_level_bid()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test7.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };
    Strategy strat(incremental_data);
    input.close();
    try{
        strat.run();
    }
    catch(std::logic_error& e)
    {
        std::cout<<e.what();
        if (string(e.what()) == "Nonexistent level has been deleted") return 0;
    };    
    throw std::logic_error("delete_inexistent_level_bid");
    return 0;
}

int delete_level_ask()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test9.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };
    Strategy strat(incremental_data);
    strat.run();
    input.close();
    if (strat.Result_ask[0] != pair<Price, Amount>(49, 1)||
        strat.Result_ask[1] != pair<Price, Amount>(49.5, 1)||
        strat.Result_ask[2] != pair<Price, Amount>(50, 1)||
        strat.Result_ask[3] != pair<Price, Amount>(51, 1)||
        strat.Result_ask[4] != pair<Price, Amount>(52, 1)||
        strat.Result_ask[5] != pair<Price, Amount>(54, 1)) throw std::logic_error("delete_level_ask");
    return 0;
}

int delete_level_bid()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test6.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };
    Strategy strat(incremental_data);
    strat.run();
    input.close();
    if (strat.Result[0] != pair<Price, Amount>(54, 1)||
        strat.Result[1] != pair<Price, Amount>(52, 1)||
        strat.Result[2] != pair<Price, Amount>(51, 1)||
        strat.Result[3] != pair<Price, Amount>(50, 1)||
        strat.Result[4] != pair<Price, Amount>(49.5, 1)||
        strat.Result[5] != pair<Price, Amount>(49, 1)) throw std::logic_error("delete_level_bid");
    return 0;
}

int change_amount_ask()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test8.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };
    Strategy strat(incremental_data);
    strat.run();
    input.close();
    if (strat.Result[0] != pair<Price, Amount>(49, 1)||
        strat.Result[1] != pair<Price, Amount>(50, 2)||
        strat.Result[2] != pair<Price, Amount>(51, 1)||
        strat.Result[3] != pair<Price, Amount>(52, 1)||
        strat.Result[4] != pair<Price, Amount>(53, 2)||
        strat.Result[5] != pair<Price, Amount>(54, 1)) throw std::logic_error("change_amount_bid");
    return 0;
}

int change_amount_bid()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test5.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };

    Strategy strat(incremental_data);
    strat.run();
    input.close();
    if (strat.Result[0] != pair<Price, Amount>(54, 1)||
        strat.Result[1] != pair<Price, Amount>(53, 2)||
        strat.Result[2] != pair<Price, Amount>(52, 1)||
        strat.Result[3] != pair<Price, Amount>(51, 1)||
        strat.Result[4] != pair<Price, Amount>(50, 1)||
        strat.Result[5] != pair<Price, Amount>(49.5, 1)) throw std::logic_error("change_amount_bid");
    return 0;
}

int unique_insert_bid_and_ask_crossed()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test1.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };
    Strategy strat(incremental_data);
    strat.run();
    input.close();
    return 0;
}

int unique_insert_ask()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test2.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };
    incremental_data.delete_last_row();
    Strategy strat(incremental_data);
    strat.run();
    input.close();
    if (strat.Result_ask[0] != pair<Price, Amount>(49, 1)||
        strat.Result_ask[1] != pair<Price, Amount>(50, 1)||
        strat.Result_ask[2] != pair<Price, Amount>(51, 1)||
        strat.Result_ask[3] != pair<Price, Amount>(52, 1)||
        strat.Result_ask[4] != pair<Price, Amount>(53, 1)||
        strat.Result_ask[5] != pair<Price, Amount>(54, 1)) throw std::logic_error("unique_insert_ask");
    return 0;
}

int unique_insert_bid()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_ob_test1.bin", ios::in | std::ios::binary);
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        std::cout<<std::endl;
    };
    Strategy strat(incremental_data);
    strat.run();
    input.close();
    if (strat.Result[0] != pair<Price, Amount>(55, 1)||
        strat.Result[1] != pair<Price, Amount>(54, 1)||
        strat.Result[2] != pair<Price, Amount>(53, 1)||
        strat.Result[3] != pair<Price, Amount>(52, 1)||
        strat.Result[4] != pair<Price, Amount>(51, 1)||
        strat.Result[5] != pair<Price, Amount>(50, 1)) throw std::logic_error("unique_insert_bid");
    return 0;
}

int main()
{
    //unique_insert_ask();
    //unique_insert_bid();
    //delete_inexistent_level_ask();
    //delete_inexistent_level_bid();
    //delete_level_ask();
    //delete_level_bid();
    //change_amount_bid();
    //change_amount_bid();
    //insert_unique_new_undefined_side();*/
    test_on_deribit();
    return 0;
}