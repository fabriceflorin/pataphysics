#!/bin/bash
/usr/bin/find /home/pi/Videos -type f  | /usr/bin/xargs /usr/bin/omxplayer --loop -b --aspect-mode full
