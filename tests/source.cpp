#include "../src/BBOExtractor.hpp"
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
    DeribitStrategy(const IncrementalData& incremental_data, const string& filename) : Worker(incremental_data), 
                                                               output(filename, ios::out)
    {
        output<<"local_timestamp,exchange_timestamp,bid_price,bid_amount,ask_price,ask_amount,spread\n";
    }
    void custom_run()
    {
        std::cout<<"running\n";
        run();
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

int test_on_deribit(const string& input_fname, const string& output_fname,  char ignore_nonexs = false)
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open(input_fname, ios::in | std::ios::binary);
    std::cout<<"Input file has been opened \n";
    int i = 0;
    while (!input.eof()){
        if (i == 2133)
            std::cout<<"A";
        if (ignore_nonexs){
            try
            {
                read_serialized_data(input, incremental_data);
            }
            catch (const std::logic_error& e)
            {
                cout<<"Error \""<<e.what()<<"\" has been ignored\n";
            };
        }
        else
        {
            read_serialized_data(input, incremental_data);
        }
        ++i;
    };
    DeribitStrategy strat(incremental_data, output_fname);
    input.close();
    strat.custom_run();
    return 1;    
};

void test_roq_data()
{
    fstream input;
    IncrementalData incremental_data(0.001);
    input.open("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_deribit.bin", ios::in | std::ios::binary);
    std::cout<<"Input file has been opened \n";
    int i = 0;
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        ++i;
    };
    DeribitStrategy strat(incremental_data, "/home/afaradzhov/OBAnalizer/python/output_tardis.csv");
    input.close();
    strat.custom_run();
}

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

int main(int argc, char *argv[])
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
    /*argc = 3;
    char in[] = "/home/afaradzhov/OBAnalizer/python/serialized_deribit_roq.bin";
    char out[] = "/home/afaradzhov/OBAnalizer/tests/datasets/output_deribit_roq_mbp_deribit.csv";
    char param3[] = "ignore_nonexs1";
    argv[1]=in;
    argv[2]=out;
    argv[3]= param3;
    if (argc==3)
            test_on_deribit(argv[1], argv[2], false);
    else if(argc == 4 && string(argv[3])=="ignore_nonexs")
            test_on_deribit(argv[1], argv[2], true);
        else 
        {
            std::cout<<"Not enough arguments";
        }*/

    //test_roq_data();
    test_on_deribit("/home/afaradzhov/OBAnalizer/tests/datasets/serialized_mbp_deribit_lon.bin", "/home/afaradzhov/OBAnalizer/tests/datasets/output_mbp_deribit_lon.csv");
    return 0;
}