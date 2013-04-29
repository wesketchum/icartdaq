#!/bin/bash

source `which setupDemoEnvironment.sh`

function launch() {
  # This expects two parameters: first the command, second the run number,
  # and third the compression level
  DemoControl.rb -s -c $1 \
    --v1720 ${ARTDAQDEMO_BR_HOST[0]},${ARTDAQDEMO_BR_PORT[0]},0 \
    --v1720 ${ARTDAQDEMO_BR_HOST[1]},${ARTDAQDEMO_BR_PORT[1]},1 \
    --eb ${ARTDAQDEMO_EB_HOST[0]},${ARTDAQDEMO_EB_PORT[0]},$3 \
    --eb ${ARTDAQDEMO_EB_HOST[1]},${ARTDAQDEMO_EB_PORT[1]},$3 \
    --data-dir $ARTDAQDEMO_DATA_DIR \
    --run-number "$2"
}

if [ $# == "0" ]; then
  echo "Usage: $0 command [options]"
  echo "  start [run]"
  echo "  init [compressionLevel (0..2)]"
  echo "  stop"
  echo "  pause"
  echo "  resume"
  echo "  shutdown"
  exit
fi

if [[ "$1" == "stop" || "$1" == "resume" || "$1" == "pause" || "$1" == "shutdown" ]]; then
  launch $1 "101" "0"
elif [[ "$1" == "start" ]]; then
  launch $1 "$2" "0"
elif [[ "$1" == "init" ]]; then
  launch $1 "101" $2
fi
