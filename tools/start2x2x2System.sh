#!/bin/bash

source `which setupDemoEnvironment.sh`

# create the configuration file for PMT
tempFile="/tmp/pmtConfig.$$"

echo "BoardReaderMain `hostname` ${ARTDAQDEMO_BR_PORT[0]}" >> $tempFile
echo "BoardReaderMain `hostname` ${ARTDAQDEMO_BR_PORT[1]}" >> $tempFile
echo "EventBuilderMain `hostname` ${ARTDAQDEMO_EB_PORT[0]}" >> $tempFile
echo "EventBuilderMain `hostname` ${ARTDAQDEMO_EB_PORT[1]}" >> $tempFile
echo "AggregatorMain `hostname` ${ARTDAQDEMO_AG_PORT[0]}" >> $tempFile
echo "AggregatorMain `hostname` ${ARTDAQDEMO_AG_PORT[1]}" >> $tempFile

# create the logfile directories, if needed
logroot="/tmp"
mkdir -p ${logroot}/pmt
mkdir -p ${logroot}/masterControl
mkdir -p ${logroot}/boardreader
mkdir -p ${logroot}/eventbuilder
mkdir -p ${logroot}/aggregator

# start PMT
pmt.rb -p ${ARTDAQDEMO_PMT_PORT} -d $tempFile --logpath ${logroot} --display ${DISPLAY}
rm $tempFile
