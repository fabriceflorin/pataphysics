#!/bin/bash
key=$(echo "$*" | awk 'NF>1{print $NF}')
case $key in
    Camera)
        ps aux | grep -ie gphoto2 | awk '{print $2}' | xargs kill -9
        cd
        if [ ! -d "PHOTOS" ]; then
            mkdir PHOTOS
        fi
        cd PHOTOS
        gphoto2 --get-all-files --new --filename "%Y%m%d-%H%M%S.%C"
    ;;
    USB)
        cd
        if [ ! -d "PHOTOS" ]; then
            mkdir PHOTOS
        fi
        USB_MOUNT_POINT=$(find /media/ -mindepth 1 -maxdepth 1 -not -empty -type d | \
                          sed 's/ /\\ /g')
        exiftool -r -d /home/pi/PHOTOS/%Y%m%d-%H%M%S.%%e '-FileName<DateTimeOriginal' \
                $USB_MOUNT_POINT >> /home/pi/transfer.log
    ;;
    Backup)
        cd
        if [ ! -d "PHOTOS" ]; then
            exit 1
        fi
        USB_MOUNT_POINT=$(find /media/ -mindepth 1 -maxdepth 1 -type d | sed 's/ /\\ /g')
        eval rsync -avhz /home/pi/PHOTOS/ $USB_MOUNT_POINT/ --log-file=/home/pi/backup.log
    ;;
    '')
        ps aux | grep -ie pimenu | awk '{print $2}' | xargs kill -9
    ;;
esac
sleep 3
