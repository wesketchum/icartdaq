#!/bin/bash

echo "Are you sure you want to install the artdaq demo in `pwd`? [y/n]"
read response
if [[ "$response" != "y" ]]; then
    echo "Aborting installation..."
    exit
fi

mkdir products
mkdir download
cd download
../downloadArtDaq-v1_07_01.sh ../products e2:eth prof

cd ..
mkdir demoBuild

if [[ ! -e artdaq-demo ]]; then
    git clone http://cdcvs.fnal.gov/projects/artdaq-demo
fi

if [[ ! -e ./setupARTDAQDEMO ]]; then
    echo "export FHICL_FILE_PATH=." > setupARTDAQDEMO
    echo "export CETPKG_INSTALL=`pwd`/products" >> setupARTDAQDEMO
    echo "export CETPKG_J=16" >> setupARTDAQDEMO
    echo "#export ARTDAQDEMO_BASE_PORT=52200" >> setupARTDAQDEMO
    echo "export DAQ_INDATA_PATH=`pwd`/artdaq-demo/test/Generators:`pwd`/artdaq-demo/inputData" >> setupARTDAQDEMO
    echo "" >> setupARTDAQDEMO
    echo "export ARTDAQDEMO_BUILD="`pwd`/demoBuild"" >> setupARTDAQDEMO
    echo "export ARTDAQDEMO_REPO="`pwd`/artdaq-demo"" >> setupARTDAQDEMO
    echo "cd \$ARTDAQDEMO_BUILD; source \$ARTDAQDEMO_REPO/ups/setup_for_development -p e2 eth" >> setupARTDAQDEMO
    echo "" >> setupARTDAQDEMO
    echo "alias rawEventDump=\"art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/rawEventDump.fcl\"" >> setupARTDAQDEMO
    echo "alias compressedEventDump=\"art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/compressedEventDump.fcl\"" >> setupARTDAQDEMO
    echo "alias compressedEventComparison=\"art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/compressedEventComparison.fcl\"" >> setupARTDAQDEMO
fi
