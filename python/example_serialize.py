#this is an example of serializer

import pandas as pd
import struct


def write_serialized_data(file, Local_timestamp, Exchange_timestamp, Price, Amount, Side, Snapshot):
    file.write(struct.pack('l', Local_timestamp))
    file.write(struct.pack('l', Exchange_timestamp))
    file.write(struct.pack('f', Price))
    file.write(struct.pack('f', Amount))
    file.write(struct.pack('i', Side))
    file.write(struct.pack('i', Snapshot))

data = pd.read_csv("../tests/datasets/deribit_incremental_book_L2_2022-08-01_BTC-PERPETUAL.csv.gz",compression='gzip');
data.sort_values(by="local_timestamp", ascending = True)
first_snapshot = data[data['is_snapshot']==True].index[0]
data = data.loc[first_snapshot:]

exchange_timestamp = data['timestamp'].to_list()
local_timestamp = data['local_timestamp'].to_list()
snapshot = data['is_snapshot'].replace({False:0, True:1}).to_list()
side = data['side'].replace({"bid":1, "ask":2}).to_list()
price = data['price'].to_list()
amount = data['amount'].to_list()
file = open("../tests/datasets/serialized_deribit.bin", "wb")
print(local_timestamp[0])
print(struct.pack('l', local_timestamp[0])[0])
for i in range(len(price)):
    file.write(struct.pack('l', local_timestamp[i]))
    file.write(struct.pack('l', exchange_timestamp[i]))
    file.write(struct.pack('f', price[i]))
    file.write(struct.pack('f', amount[i]))
    file.write(struct.pack('i', side[i]))
    file.write(struct.pack('i', snapshot[i]))
file.close()
