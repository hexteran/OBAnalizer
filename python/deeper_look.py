import pandas
import pandas as pd
import struct

def write_serialized_data(file, Local_timestamp, Exchange_timestamp, Price, Amount,dummy, Side, Snapshot):
    file.write(struct.pack('l', Local_timestamp))
    file.write(struct.pack('l', Exchange_timestamp))
    file.write(struct.pack('f', Price))
    file.write(struct.pack('f', Amount))
    file.write(struct.pack('i', Side))
    file.write(struct.pack('i', Snapshot))

data = pd.read_csv("../tests/datasets/deribit_incremental_book_L2_2022-08-01_BTC-PERPETUAL.csv.gz",compression='gzip');
data.sort_values(by="local_timestamp", ascending = True)
#first_snapshot = data[data['is_snapshot']==True].index[0]
#data = data.loc[first_snapshot:]
data = data[data['price']==23303.5]
print(data) 
