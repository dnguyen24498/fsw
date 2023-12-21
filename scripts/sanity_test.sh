#!/bin/bash

# Set the desired number of loop iterations
num_iterations=100

# Initialize the counter
counter=0

while [ $counter -lt $num_iterations ]
do
    echo -ne '\x4b\x55\x00\x00\xff\xff\x01\x00\xff\xff\xff\x00\x01\x00\x00\xff\xff\xff' > /dev/virtualcom1
    read -t 0.1 -s
    
     # Increment the counter
    ((counter++))
done
echo "Test done!"