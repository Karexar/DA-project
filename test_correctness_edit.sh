#!/bin/bash
#
# Tests the correctness of the Uniform Reliable Broadcast application.
#
# This is an example script that shows the general structure of the
# test. The details and parameters of the actual test might differ.
#

# time to wait for correct processes to broadcast all messages (in seconds)
# (should be adapted to the number of messages to send)
time_to_finish=2

init_time=2

# configure lossy network simulation
sudo tc qdisc change dev lo root netem delay 50ms 200ms loss 10% 25% reorder 25% 50%

# compile (should output: da_proc)
make

echo "5
3 127.0.0.1 11001
4 127.0.0.1 11002
5 127.0.0.1 11003
6 127.0.0.1 11004
7 127.0.0.1 11005" > membership

# start 5 processes, each broadcasting 100 messages
for i in `seq 3 7`
do
    ./da_proc $i membership 100 &
    da_proc_id[$i]=$!
done

# leave some time for process initialization
sleep $init_time

# do some nasty stuff like process crashes and delays
# example:
kill -STOP "${da_proc_id[5]}" # pause process 5
sleep 1
kill -TERM "${da_proc_id[6]}" # crash process 6
da_proc_id[2]=""
kill -CONT "${da_proc_id[5]}" # resume process 5

# start broadcasting
#for i in `seq 3 7`
#do
#    if [ -n "${da_proc_id[$i]}" ]; then
#	kill -USR2 "${da_proc_id[$i]}"
#    fi
#done

kill -USR2 "${da_proc_id[7]}"

sleep 1

kill -USR2 "${da_proc_id[3]}"

sleep 1

kill -USR2 "${da_proc_id[4]}"
kill -USR2 "${da_proc_id[5]}"

sleep 0.01

# do some more nasty stuff
# example:
kill -TERM "${da_proc_id[6]}" # crash process 6
da_proc_id[4]=""
kill -STOP "${da_proc_id[3]}" # pause process 3
sleep 0.5
kill -CONT "${da_proc_id[3]}" # resume process 3

# leave some time for the correct processes to broadcast all messages
sleep $time_to_finish

# stop all processes
for i in `seq 3 7`
do
    if [ -n "${da_proc_id[$i]}" ]; then
	kill -TERM "${da_proc_id[$i]}"
    fi
done

# wait until all processes stop
for i in `seq 3 7`
do
    if [ -n "${da_proc_id[$i]}" ]; then
	    wait "${da_proc_id[$i]}"
    fi
done

# check logs for correctness
./check_output.sh 3 5 7

echo "Correctness test done."
