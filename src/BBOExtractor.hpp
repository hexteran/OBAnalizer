#include "OBAnalizer.h"
#include <fstream>
#include <math.h>

struct BBOCollector : public Worker
//This Worker collects BBO data
{
    fstream output;
    BBOCollector(const IncrementalData& incremental_data, const string& filename) : Worker(incremental_data), 
                                                               output(filename, ios::out)
    {
        output<<"local_timestamp,exchange_timestamp,bid_price,bid_amount,ask_price,ask_amount,spread\n";
    }
    bool custom_run()
    {
        std::cout<<"Worker is running\n";
        run();
        return true;
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

bool read_serialized_data( fstream& input, IncrementalData& incremental_data)
{
    Timestamp local_timestamp;
    Timestamp exchange_timestamp;
    Price price;
    Amount amount;
    int side, snapshot; 
    input.read((char*)&local_timestamp, sizeof(local_timestamp));
    input.read((char*)&exchange_timestamp, sizeof(exchange_timestamp));
    input.read((char*)&price, sizeof(price));
    input.read((char*)&amount, sizeof(amount));
    input.read((char*)&side, sizeof(side));
    input.read((char*)&snapshot, sizeof(snapshot));
    incremental_data.add_row(local_timestamp, exchange_timestamp, price, amount, (char)side, (bool)snapshot);
    return true;
}

void run(BBOCollector& strat, bool ignore_nonexistent)
{
    while (true){
        try
        {
            if (strat.custom_run())
                return;
        }
        catch(OBException& ex)
        {
            std::cout<<"\n\nOB EXCEPTION:\n"<<ex.what();
            if (ex.type() == Exception_Type::Nonexistent_level && !ignore_nonexistent)
            {
                std::cout<<"\nQuiting...\n";
                return;
            };
        };
    };
}

void process_file(string input_fname, string output_fname, bool ignore_nonexistent = false)
{
    fstream input;
    IncrementalData incremental_data;
    input.open(input_fname, ios::in | std::ios::binary);
    if (!input.is_open())
    {
        cout<<"Can't open file "<<output_fname;
        return;
    };
    cout<<"Input file has been opened \n";
    int i = 0;
    while (!input.eof()){
        read_serialized_data(input, incremental_data);
        ++i;
    };

    BBOCollector strat(incremental_data, output_fname);
    input.close();
    run(strat, ignore_nonexistent);
    //strat.custom_run();

    cout<<"\nThe deed is done\n\n";
};

inline void show_help()
{
    cout<<"use\n--output_file to define comma-delimetered .csv file\n--input_file to define .bin file of input values\n--ignore_nonexistent to ignore deletion of nonexistent price levels\n";
};

void parse_cmd(int argc, char *argv[])
{
    string input_fname = "basic_input_file.bin";
    string output_fname = "basic_output_file.csv";
    bool ignore_nonexistent = false;
    if (argc<2)
    {
        show_help();
        return;
    }
    for (int i = 1; i < argc; i++)
    {
        if ((string)argv[i]=="--input_file" && i+1<=argc)
        {
            ++i;
            input_fname = argv[i];          
        }
        else if ((string)argv[i]=="--output_file" && i+1<=argc)
        {
            ++i;
            output_fname = argv[i];
        }
        else if ((string)argv[i]=="--ignore_nonexistent" && i+1<=argc)
        {
            ignore_nonexistent = true;
        }
        else
        {
            show_help();
            return;
        };      
    };
    process_file(input_fname, output_fname, ignore_nonexistent);

};