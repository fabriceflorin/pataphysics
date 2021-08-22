#!/bin/bash

# Start out with a transition
# /usr/bin/omxplayer -b "/home/pi/Videos/Night.mp4"

# Continually loop and pick a random file to play back.
for (( ; ; ))
do
   /usr/bin/find /home/pi/Videos -size +1M -type f | /usr/bin/shuf -n1 | /usr/bin/xargs -d '\n' /usr/bin/omxplayer -b
#   sleep 1
done

