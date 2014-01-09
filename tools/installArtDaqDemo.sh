#!/bin/bash

# this is only here in case this script is sourced instead of run in its own shell
sh -c "[ `ps $$ | grep bash | wc -l` -gt 0 ] || { echo 'Please switch to the bash shell before installing the artdaq-demo.'; exit 1; }" || exit

echo "Are you sure you want to install the artdaq demo in `pwd`? [y/n]"
read response
if [[ "$response" != "y" ]]; then
    echo "Aborting installation..."
    exit
fi

test -d build_artdaq      || mkdir build_artdaq  # This is where we will build artdaq
test -d build_artdaq-demo || mkdir build_artdaq-demo  # This is where we will build artdaq-demo

if [[ ! -e ./setupARTDAQDEMO ]]; then
    cat >setupARTDAQDEMO <<-EOF
	echo # This script is intended to be sourced.

	sh -c "[ \`ps \$\$ | grep bash | wc -l\` -gt 0 ] || { echo 'Please switch to the bash shell before running the artdaq-demo.'; exit; }" || exit

	source `pwd`/products/setup

	export FHICL_FILE_PATH=.
	export CETPKG_INSTALL=`pwd`/products
	export CETPKG_J=16
	#export ARTDAQDEMO_BASE_PORT=52200
	export DAQ_INDATA_PATH=`pwd`/artdaq-demo/test/Generators:`pwd`/artdaq-demo/inputData

	export ARTDAQDEMO_BUILD="`pwd`/build_artdaq-demo"" >> setupARTDAQDEMO
	export ARTDAQDEMO_REPO="`pwd`/artdaq-demo"" >> setupARTDAQDEMO

	alias rawEventDump="art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/rawEventDump.fcl"
	alias compressedEventDump="art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/compressedEventDump.fcl"
	alias compressedEventComparison="art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/compressedEventComparison.fcl"
	EOF
    #
fi
. ./setupARTDAQDEMO


# Get artdaq from central git repository
test -d artdaq || git clone http://cdcvs.fnal.gov/projects/artdaq
cd artdaq
git fetch origin
git checkout v1_05_01
cd ../build_artdaq
echo IN $PWD: about to . ../artdaq/ups/setup_for_development
. ../artdaq/ups/setup_for_development -p e4 eth
echo FINISHED ../artdaq/ups/setup_for_development
buildtool -p
cd ../products
tar xf ../build_artdaq/artdaq-1.05.01-slf6-x86_64-e4-eth-prof.tar.bz2
cd ..



echo "Building artdaq-demo..."
cd $ARTDAQDEMO_BUILD
. $ARTDAQDEMO_REPO/ups/setup_for_development -p e4 eth
buildtool

echo "Installation and build complete; please see https://cdcvs.fnal.gov/redmine/projects/artdaq-demo/wiki/Running_a_sample_artdaq-demo_system for instructions on how to run"


