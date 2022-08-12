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


#insert_unique_new bid only
file = open("../tests/datasets/serialized_ob_test1.bin", "wb");
write_serialized_data(file,0,0,50,1,1,1)
write_serialized_data(file,1,0,51,1,1,1)
write_serialized_data(file,2,0,52,1,1,1)
write_serialized_data(file,4,0,54,1,1,1)
write_serialized_data(file,5,0,49,1,1,1)
write_serialized_data(file,6,0,55,1,1,1)
file.close()

#insert_unique_new ask only
file = open("../tests/datasets/serialized_ob_test2.bin", "wb");
write_serialized_data(file,0,0,50,1,2,1)
write_serialized_data(file,1,0,51,1,2,1)
write_serialized_data(file,2,0,52,1,2,1)
write_serialized_data(file,3,0,53,1,2,1)
write_serialized_data(file,4,0,54,1,2,1)
write_serialized_data(file,5,0,49,1,2,1)
write_serialized_data(file,6,0,55,1,2,1)
file.close()

#insert_unique_new bid and ask crossed
file = open("../tests/datasets/serialized_ob_test3.bin", "wb");
write_serialized_data(file,0,0,50,1,2,1)
write_serialized_data(file,1,0,51,1,1,1)
write_serialized_data(file,2,0,52,1,2,1)
write_serialized_data(file,3,0,53,1,2,1)
write_serialized_data(file,4,0,54,1,2,1)
write_serialized_data(file,5,0,49,1,1,1)
write_serialized_data(file,6,0,55,1,2,1)
file.close()

#insert_unique_new undefined side
file = open("../tests/datasets/serialized_ob_test4.bin", "wb");
write_serialized_data(file,0,0,50,1,1,1)
write_serialized_data(file,1,0,51,1,1,1)
write_serialized_data(file,2,0,52,1,1,1)
write_serialized_data(file,3,0,53,1,0,1)
write_serialized_data(file,4,0,54,1,1,1)
write_serialized_data(file,5,0,49,1,1,1)
write_serialized_data(file,6,0,55,1,1,1)
file.close()

#change_amount bid
file = open("../tests/datasets/serialized_ob_test5.bin", "wb");
write_serialized_data(file,0,0,50,1,1,1)
write_serialized_data(file,1,0,51,1,1,1)
write_serialized_data(file,2,0,52,1,1,1)
write_serialized_data(file,3,0,53,1,1,1)
write_serialized_data(file,4,0,54,1,1,1)
write_serialized_data(file,5,0,49,1,1,1)
write_serialized_data(file,6,0,53,2,1,1)
write_serialized_data(file,7,0,49.5,1,1,1)
file.close()

#change_amount ask
file = open("../tests/datasets/serialized_ob_test8.bin", "wb");
write_serialized_data(file,0,0,50,1,2,1)
write_serialized_data(file,1,0,51,1,2,1)
write_serialized_data(file,2,0,52,1,2,1)
write_serialized_data(file,3,0,53,1,2,1)
write_serialized_data(file,4,0,54,1,2,1)
write_serialized_data(file,5,0,49,1,2,1)
write_serialized_data(file,6,0,53,2,2,1)
file.close()

#delete_level bid
file = open("../tests/datasets/serialized_ob_test6.bin", "wb");
write_serialized_data(file,0,0,50,1,1,1)
write_serialized_data(file,1,0,51,1,1,1)
write_serialized_data(file,2,0,52,1,1,1)
write_serialized_data(file,3,0,53,1,1,1)
write_serialized_data(file,4,0,54,1,1,1)
write_serialized_data(file,5,0,49,1,1,1)
write_serialized_data(file,5,0,49.5,1,1,1)
write_serialized_data(file,6,0,53,0,1,1)
file.close()

#delete_level ask
file = open("../tests/datasets/serialized_ob_test9.bin", "wb");
write_serialized_data(file,0,0,50,1,2,1)
write_serialized_data(file,1,0,51,1,2,1)
write_serialized_data(file,2,0,52,1,2,1)
write_serialized_data(file,3,0,53,1,2,1)
write_serialized_data(file,4,0,54,1,2,1)
write_serialized_data(file,5,0,49,1,2,1)
write_serialized_data(file,5,0,49.5,1,2,1)
write_serialized_data(file,6,0,53,0,2,1)
file.close()

#delete_inexistent_level bid
file = open("../tests/datasets/serialized_ob_test7.bin", "wb");
write_serialized_data(file,0,0,50,1,1,1)
write_serialized_data(file,1,0,51,1,1,1)
write_serialized_data(file,2,0,52,1,1,1)
write_serialized_data(file,4,0,54,1,1,1)
write_serialized_data(file,5,0,49,1,1,1)
write_serialized_data(file,5,0,49.5,1,1,1)
write_serialized_data(file,6,0,53,0,1,1)
file.close()

#delete_inexistent_level ask
file = open("../tests/datasets/serialized_ob_test10.bin", "wb");
write_serialized_data(file,0,0,50,1,2,1)
write_serialized_data(file,1,0,51,1,2,1)
write_serialized_data(file,2,0,52,1,2,1)
write_serialized_data(file,4,0,54,1,2,1)
write_serialized_data(file,5,0,49,1,2,1)
write_serialized_data(file,5,0,49.5,1,2,1)
write_serialized_data(file,6,0,53,0,2,1)
file.close()


