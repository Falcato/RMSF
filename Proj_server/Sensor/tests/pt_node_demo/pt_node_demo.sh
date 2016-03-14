#------------------------------------------------------------------------------
# $Id: pt_node_demo.sh 649 2011-11-30 16:09:30Z jose.goncalves $
#------------------------------------------------------------------------------
#!/bin/sh

DEVICE=/dev/video0
IP=192.168.0.254
PORT=5004
FPS=10
SIZE=cif
CODEC=h263 # mjpeg, mpeg4, h263, h263p
#CODEC="libx264 -profile baseline -level 1.2 -preset ultrafast -tune zerolatency -x264opts keyint=${FPS}"
OPT="?pkt_size=1315"
#LOG="-loglevel debug"

IR_DEVICE=/dev/ttySAC0
IR_PORT=12345

GPIO_PIR=197
GPIO_LIGHT=166

kill_all_proc() {
	if [ ${PID_PIR} -ne 0 ]; then
		kill ${PID_PIR} 2> /dev/null
	fi
	killall ir_stream 2>/dev/null
	killall ffmpeg 2> /dev/null
}

check_proc() {
	pid=$!
	usleep 100000
	kill -0 ${pid} 2> /dev/null
	if [ $? -ne 0 ]; then
		echo "$1"
		exit 1  
	fi
}


if [ ! -e ${DEVICE} ] ; then
	echo "ERROR: No webcam connected!"
	exit 1
fi

PID_PIR=0

kill_all_proc

pir_light.sh ${GPIO_PIR} ${GPIO_LIGHT} </dev/null >/dev/null 2>&1 &
PID_PIR=$!
check_proc "ERROR: Unable to control PIR and/or LEDs!"

ir_stream -d ${IR_DEVICE} -h ${IP_GW} -p ${IR_PORT} </dev/null >/dev/null 2>&1 &
check_proc "ERROR: Unable to access IR camera!"

if [ "${CODEC}" = "mjpeg" ] ; then
	PARAMS="-vcodec ${CODEC} -r ${FPS} -s ${SIZE} -i ${DEVICE} -vcodec copy -f ${CODEC} udp://${IP}:${PORT}${OPT}"
else
	PARAMS="-vcodec rawvideo -r ${FPS} -s ${SIZE} -i ${DEVICE} -vcodec ${CODEC} -f rtp rtp://${IP}:${PORT}${OPT}"
fi
ffmpeg ${LOG} -f video4linux2 ${PARAMS} </dev/null >/dev/null 2>&1 &

cam_control ${DEVICE} ${GPIO_LIGHT}

kill_all_proc

