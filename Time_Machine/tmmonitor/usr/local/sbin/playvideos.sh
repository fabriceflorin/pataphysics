#!/bin/bash

# Start out with a transition
/usr/bin/omxplayer -b "/videos/Time Travel 1 - Big Bang 1.mp4"

# Continually loop and pick a random file to play back.
#for (( ; ; ))
#do
#   /usr/bin/find /videos -type f  | /usr/bin/shuf -n1 | /usr/bin/xargs -d '\n' /usr/bin/omxplayer -b
#   sleep 1
#done
