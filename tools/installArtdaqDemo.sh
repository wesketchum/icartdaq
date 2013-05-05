#!/bin/bash

echo "Are you sure you want to install the artdaq demo in `pwd`? [y/n]"
read response
if [[ "$response" != "y" ]]; then
    echo "Aborting installation..."
    exit
fi

mkdir download
cd download
wget -nv -N http://oink.fnal.gov/distro/private-products-setup/upsfiles-dir.tar.gz
wget -nv -N http://oink.fnal.gov/distro/relocatable-ups/ups-upd-4.9.7-slf6-x86_64.tar.bz2
wget -nv -N http://oink.fnal.gov/distro/art/art_externals-0.06.04-slf6-x86_64-e2-prof.tar.bz2
wget -nv -N http://oink.fnal.gov/distro/art/art_suite-1.07.01-slf6-x86_64-e2-prof.tar.bz2
wget -nv -N http://oink.fnal.gov/distro/packages/artdaq-1.00.00-slf6-x86_64-e2-eth-prof.tar.bz2
wget -nv -N http://oink.fnal.gov/distro/packages/mpich-3.0.2-slf6-x86_64-prof.tar.bz2
wget -nv -N http://oink.fnal.gov/distro/packages/xmlrpc_c-1.25.18-slf6-x86_64-e2-prof.tar.bz2
wget -nv -N http://oink.fnal.gov/distro/packages/gccxml-0.9.20130503-slf6-x86_64-gcc47.tar.bz2

cd ..
mkdir products
cd products
tar -xvf ../download/upsfiles-dir.tar.gz
tar -xvf ../download/ups-upd-4.9.7-slf6-x86_64.tar.bz2
tar -xvf ../download/art_externals-0.06.04-slf6-x86_64-e2-prof.tar.bz2
tar -xvf ../download/art_suite-1.07.01-slf6-x86_64-e2-prof.tar.bz2
tar -xvf ../download/artdaq-1.00.00-slf6-x86_64-e2-eth-prof.tar.bz2
tar -xvf ../download/mpich-3.0.2-slf6-x86_64-prof.tar.bz2
tar -xvf ../download/xmlrpc_c-1.25.18-slf6-x86_64-e2-prof.tar.bz2
tar -xvf ../download/gccxml-0.9.20130503-slf6-x86_64-gcc47.tar.bz2

cd ..
mkdir demoBuild

if [[ ! -e artdaq-demo ]]; then
    git clone ssh://p-artdaq-demo@cdcvs.fnal.gov/cvs/projects/artdaq-demo
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
