#include "BBOExtractor.hpp"
void test1(){
    L3OrderBookSnapshot ob;
    ob.new_order(1,1,100,10, BID, "1a");
    ob.new_order(1,1,100,10, BID, "2a");
    ob.new_order(1,1,100,10, BID, "3a");
    OrderBucket first(0), *second, *third;
    if(ob.get_order_bucket_by_level(0, BID, &first))
    {
        std::cout<<"GPR";
        std::cout<<(first).price<<"::";
        for(auto it = (first).orders.begin(); it!=(first).orders.end(); it++)
        {
            std::cout<<it->amount<<"-"<<it->orderID<<" ";
        };
        std::cout<<"\n";
    };

    int l = 10;
}
void test2(){
    L3OrderBookSnapshot ob;
    char side = ASK;
    ob.new_order(1,1,100,10, side, "1a");
    ob.new_order(2,1,101,10, side, "1b");
    ob.new_order(3,1,100,10, side, "2a");
    ob.new_order(4,1,100,10, side, "3a");
    ob.new_order(5,1,99,10, side, "1c");
    ob.new_order(6,1,101,10, side, "2b");
    ob.new_order(7,1,101,10, side, "3b");
    ob.new_order(8,1,99,10, side, "2c");
    ob.new_order(9,1,99,10, side, "3c");
    /*ob.cancel_order("3c");
    ob.cancel_order("2b");
    ob.new_order(9,1,99,10, side, "3c");
    ob.cancel_order("1b");
    ob.cancel_order("2b");
    ob.cancel_order("3b");
    ob.new_order(6,1,101,10, side, "4b");
    ob.new_order(6,1,101,10, side, "2b");
    ob.cancel_order("1c");
    ob.cancel_order("2c");
    ob.cancel_order("3c");
    ob.cancel_order("1a");
    ob.cancel_order("2a");
    ob.cancel_order("3a");
    ob.cancel_order("1b");
    ob.cancel_order("2b");
    ob.cancel_order("4b");
    ob.new_order(8,1,198.5,10, side, "2c");
    ob.new_order(9,1,198,10, side, "3c");// */
    OrderBucket first;
    for (int i = 0; ob.get_order_bucket_by_level(i, side, &first);i++)
    {
        std::cout<<i<<"- "<<(first).price<<":: ";
        for(auto it = (first).orders.begin(); it!=(first).orders.end(); it++)
        {
            std::cout<<it->amount<<"-"<<it->orderID<<' ';
        };
        std::cout<<"\n";
    };
    int l=10;
}
void test3(){
    L3OrderBookSnapshot ob;
    char side = BID;
    ob.new_order(1,1,100,10, side, "1a");
    ob.new_order(2,1,101,10, side, "1b");
    ob.new_order(3,1,100,10, side, "2a");
    ob.new_order(4,1,100,10, side, "3a");
    ob.new_order(5,1,99,10, side, "1c");
    ob.new_order(6,1,101,10, side, "2b");
    ob.new_order(7,1,101,10, side, "3b");
    ob.new_order(8,1,99,10, side, "2c");
    ob.new_order(9,1,99,10, side, "3c");
    /*ob.cancel_order("3c");
    ob.cancel_order("2b");
    ob.new_order(9,1,99,10, side, "3c");
    ob.cancel_order("1b");
    ob.cancel_order("2b");
    ob.cancel_order("3b");
    ob.new_order(6,1,101,10, side, "4b");
    ob.new_order(6,1,101,10, side, "2b");
    ob.cancel_order("1c");
    ob.cancel_order("2c");
    ob.cancel_order("3c");
    ob.cancel_order("1a");
    ob.cancel_order("2a");
    ob.cancel_order("3a");
    ob.cancel_order("1b");
    ob.cancel_order("2b");
    ob.cancel_order("4b");
    ob.new_order(8,1,198.5,10, side, "2c");
    ob.new_order(9,1,198,10, side, "3c");// */
    bool a = ob.change_order(10, 1, 105, 15, BID, "3c");
    ob.change_order(10, 1, 105, 15, BID, "2c");
    ob.change_order(10, 1, 105, 15, BID, "1c");
    OrderBucket first;
    for (int i = 0; ob.get_order_bucket_by_level(i, side, &first);i++)
    {
        std::cout<<i<<"- "<<(first).price<<":: ";
        for(auto it = (first).orders.begin(); it!=(first).orders.end(); it++)
        {
            std::cout<<it->amount<<"-"<<it->orderID<<' ';
        };
        std::cout<<"\n";
    };
    int l=10;
}
int main(int argc, char *argv[])
{
    //parse_cmd(argc, argv);
    //test1();
    //test2();
    test3();
    return 0;
};