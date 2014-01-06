#!/bin/bash

# this is only here in case this script is sourced instead of run in its own shell
sh -c "[ `ps $$ | grep bash | wc -l` -gt 0 ] || { echo 'Please switch to the bash shell before installing the artdaq-demo.'; exit 1; }" || exit

echo "Are you sure you want to install the artdaq demo in `pwd`? [y/n]"
read response
if [[ "$response" != "y" ]]; then
    echo "Aborting installation..."
    exit
fi

mkdir -p products
mkdir -p download
cd download
../downloadArtDaq-v1_07_01.sh ../products e4:eth prof

cd ..
mkdir -p demoBuild

if [[ ! -e artdaq-demo ]]; then
    git clone http://cdcvs.fnal.gov/projects/artdaq-demo
fi

if [[ -e ./setupARTDAQDEMO ]]; then
    echo "Do you want to over-write the existing setupARTDAQDEMO script? [y/n]"
    read response
    if [[ "$response" != "y" ]]; then
        echo "Aborting..."
        exit
    fi
fi

echo "# This script is intended to be sourced." > setupARTDAQDEMO
echo "" >> setupARTDAQDEMO
echo "sh -c \"[ \`ps \$\$ | grep bash | wc -l\` -gt 0 ] || { echo 'Please switch to the bash shell before running the artdaq-demo.'; exit \" || exit" >> setupARTDAQDEMO
echo "" >> setupARTDAQDEMO
echo "source `pwd`/products/setup" > setupARTDAQDEMO
echo "" >> setupARTDAQDEMO
echo "export FHICL_FILE_PATH=." >> setupARTDAQDEMO
echo "export CETPKG_INSTALL=`pwd`/products" >> setupARTDAQDEMO
echo "export CETPKG_J=16" >> setupARTDAQDEMO
echo "#export ARTDAQDEMO_BASE_PORT=52200" >> setupARTDAQDEMO
echo "export DAQ_INDATA_PATH=`pwd`/artdaq-demo/test/Generators:`pwd`/artdaq-demo/inputData" >> setupARTDAQDEMO
echo "" >> setupARTDAQDEMO
echo "export ARTDAQDEMO_BUILD="`pwd`/demoBuild"" >> setupARTDAQDEMO
echo "export ARTDAQDEMO_REPO="`pwd`/artdaq-demo"" >> setupARTDAQDEMO
echo "cd \$ARTDAQDEMO_BUILD; source \$ARTDAQDEMO_REPO/ups/setup_for_development -p e4 eth" >> setupARTDAQDEMO
echo "" >> setupARTDAQDEMO
echo "alias rawEventDump=\"art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/rawEventDump.fcl\"" >> setupARTDAQDEMO
echo "alias compressedEventDump=\"art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/compressedEventDump.fcl\"" >> setupARTDAQDEMO
echo "alias compressedEventComparison=\"art -c `pwd`/artdaq-demo/artdaq-demo/ArtModules/fcl/compressedEventComparison.fcl\"" >> setupARTDAQDEMO
