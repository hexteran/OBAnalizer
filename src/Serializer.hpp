#include "OBAnalizer.h"
#include <fstream>
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