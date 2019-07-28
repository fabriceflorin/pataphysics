#processes=`ps -ef | grep [o]mxplayer | awk '{print $2}'`
#for process in $processes
#do
  #kill -9 $process
#done
pkill mxplayer
