./controller &
./node 4 &
./node 5 receiver 1 &
./node 9 receiver 0 &
./node 0 sender “this_is_node_0_multicast_message” &
./node 3 &
sleep 40s 
./node 1 sender "node_1_sending" &
