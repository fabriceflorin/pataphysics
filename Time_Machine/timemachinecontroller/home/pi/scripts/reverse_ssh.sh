#!/bin/sh
# 
# Nails up a reverse tunnel so we can ssh into a box that is behind NAT.
# 

# while [ 1 ]; do
  # /usr/bin/ssh -p 22000 -C -o ConnectTimeout=5 -o ConnectionAttempts=300 -o ServerAliveInterval=30 -o ServerAliveCountMax=3 -i /home/pi/.ssh/myawsEC2.pem -R 22001:localhost:22 ubuntu@ec2-13-57-209-129.us-west-1.compute.amazonaws.com -N
  # /usr/bin/ssh -p 22000 -i /home/pi/.ssh/myawsEC2.pem -C -o ConnectTimeout=5 -o ConnectionAttempts=300 -o ServerAliveInterval=30 -o ServerAliveCountMax=3 -f -N -T -R 22001:localhost:22 ubuntu@ec2-13-57-209-129.us-west-1.compute.amazonaws.com 
  #sleep 60
#done

#/usr/bin/autossh -f -nNT -i /home/pi/.ssh/myawsEC2.pem -R 22001:localhost:22 -p 22000 ubuntu@ec2-13-57-209-129.us-west-1.compute.amazonaws.com
#/usr/bin/autossh -f -nNT -i /home/pi/.ssh/myawsEC2.pem -R 22001:localhost:22 -p 22000 timemachine@ec2-13-57-209-129.us-west-1.compute.amazonaws.com
#/usr/bin/autossh -f -nNT -R 22001:localhost:22 timemachine@ec2-13-57-209-129.us-west-1.compute.amazonaws.com -p22000
#/usr/bin/autossh -f -nNT -R 22001:localhost:22 timemachine@ec2-54-183-135-119.us-west-1.compute.amazonaws.com -p22000
/usr/bin/autossh -f -nNT -R 22001:localhost:22 timemachine@13.52.155.197 -p22000

