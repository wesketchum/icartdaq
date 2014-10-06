#!/bin/bash

env_opts_var=`basename $0 | sed 's/\.sh$//' | tr 'a-z-' 'A-Z_'`_OPTS
USAGE="\
  usage: `basename $0` [options] <demo_products_dir/> <artdaq-demo/>
example: `basename $0` products artdaq-demo --run-demo
<demo_products>    where products were installed (products/)
<artdaq-demo_root> directory where artdaq-demo was cloned into.
--run-demo   runs the demo
--debug      perform a debug build
-c           \"clean\" build dirs -- may be need during development
Currently this script will clone (if not already cloned) artdaq
along side of the artdaq-demo dir.
Also it will create, if not already created, build directories
for artdaq and artdaq-demo.
"
# Process script arguments and options
eval env_opts=\${$env_opts_var-} # can be args too
eval "set -- $env_opts \"\$@\""
op1chr='rest=`expr "$op" : "[^-]\(.*\)"`   && set -- "-$rest" "$@"'
op1arg='rest=`expr "$op" : "[^-]\(.*\)"`   && set --  "$rest" "$@"'
reqarg="$op1arg;"'test -z "${1+1}" &&echo opt -$op requires arg. &&echo "$USAGE" &&exit'
args= do_help= opt_v=0
while [ -n "${1-}" ];do
    if expr "x${1-}" : 'x-' >/dev/null;then
        op=`expr "x$1" : 'x-\(.*\)'`; shift   # done with $1
        leq=`expr "x$op" : 'x-[^=]*\(=\)'` lev=`expr "x$op" : 'x-[^=]*=\(.*\)'`
        test -n "$leq"&&eval "set -- \"\$lev\" \"\$@\""&&op=`expr "x$op" : 'x\([^=]*\)'`
        case "$op" in
        \?*|h*)    eval $op1chr; do_help=1;;
        v*)        eval $op1chr; opt_v=`expr $opt_v + 1`;;
        x*)        eval $op1chr; set -x;;
        -run-demo) opt_run_demo=--run-demo;;
        -debug)    opt_debug=--debug;;
        -c*)       eval $op1chr; opt_clean=1;;
        *)         echo "Unknown option -$op"; do_help=1;;
        esac
    else
        aa=`echo "$1" | sed -e"s/'/'\"'\"'/g"` args="$args '$aa'"; shift
    fi
done
eval "set -- $args \"\$@\""; unset args aa

test -n "${do_help-}" -o $# -ne 2 && echo "$USAGE" && exit

products_dir=`cd "$1" >/dev/null;pwd`
artdaq_demo_dir=`cd "$2" >/dev/null;pwd`
demo_dir=`dirname "$artdaq_demo_dir"`

export CETPKG_INSTALL=$products_dir
export CETPKG_J=16

test -d "$demo_dir/build_artdaq-core" || mkdir "$demo_dir/build_artdaq-core" 
test -d "$demo_dir/build_artdaq"      || mkdir "$demo_dir/build_artdaq"
test -d "$demo_dir/build_artdaq-core-demo" || mkdir "$demo_dir/build_artdaq-core-demo" 
test -d "$demo_dir/build_artdaq-demo" || mkdir "$demo_dir/build_artdaq-demo" 

if [[ -n "${opt_debug:-}" ]];then
    build_arg="d"
else
    build_arg="p"
fi

# Commit 52d6e7b4527dce8a86b7bcaf5970d45013373b89, from 9/15/14,
# updates artdaq core v1_04_00 s.t. it includes the BuildInfo template

test -d artdaq-core || git clone http://cdcvs.fnal.gov/projects/artdaq-core
cd artdaq-core
git fetch origin
git checkout 52d6e7b4527dce8a86b7bcaf5970d45013373b89
cd ../build_artdaq-core
echo IN $PWD: about to . ../artdaq-core/ups/setup_for_development
. $products_dir/setup
. ../artdaq-core/ups/setup_for_development -${build_arg} e5 s3
echo FINISHED ../artdaq-core/ups/setup_for_development
buildtool ${opt_clean+-c} -i
cd ..


# Commit b520cf663e5325cd9b4378dfd29697a9f6bd9e35, from 9/17/14,
# updates artdaq-core-demo to compile with the e5:s3 option (against
# artdaq-core v1_04_00, etc.) and adds a traits class supplying build info

test -d artdaq-core-demo || git clone http://cdcvs.fnal.gov/projects/artdaq-core-demo
cd artdaq-core-demo
git fetch origin
git checkout 208a1052b352863d7f1762bdc332d4a40a5e9bce
cd ../build_artdaq-core-demo
echo IN $PWD: about to . ../artdaq-core-demo/ups/setup_for_development
. $products_dir/setup
. ../artdaq-core-demo/ups/setup_for_development -${build_arg} e5 s3
echo FINISHED ../artdaq-core-demo/ups/setup_for_development
buildtool ${opt_clean+-c} -i
cd ..

# artdaq commit f0f0c5eb950f5a53e06aee564975357c4bc5da7e, from
# 9/16/14, includes both the merging of the buildinfo branch and the
# timestamps branch

test -d artdaq || git clone http://cdcvs.fnal.gov/projects/artdaq
cd artdaq
git fetch origin
git checkout f0f0c5eb950f5a53e06aee564975357c4bc5da7e
cd ../build_artdaq
echo IN $PWD: about to . ../artdaq/ups/setup_for_development
. $products_dir/setup
. ../artdaq/ups/setup_for_development -${build_arg} e5 s3 eth
echo FINISHED ../artdaq/ups/setup_for_development
buildtool ${opt_clean+-c} -i

cd $demo_dir >/dev/null
if [[ ! -e ./setupARTDAQDEMO ]]; then
    cat >setupARTDAQDEMO <<-EOF
	echo # This script is intended to be sourced.

	sh -c "[ \`ps \$\$ | grep bash | wc -l\` -gt 0 ] || { echo 'Please switch to the bash shell before running the artdaq-demo.'; exit; }" || exit

	source $products_dir/setup

	export CETPKG_INSTALL=$products_dir
	export CETPKG_J=16
	#export ARTDAQDEMO_BASE_PORT=52200
	export DAQ_INDATA_PATH=$artdaq_demo_dir/test/Generators:$artdaq_demo_dir/inputData

	export ARTDAQDEMO_BUILD="$demo_dir/build_artdaq-demo"
	export ARTDAQDEMO_REPO="$artdaq_demo_dir"
	export FHICL_FILE_PATH=.:\$ARTDAQDEMO_REPO/tools/fcl:\$FHICL_FILE_PATH

	echo changing directory to \$ARTDAQDEMO_BUILD
	cd \$ARTDAQDEMO_BUILD  # note: next line adjusts PATH based one cwd
	. \$ARTDAQDEMO_REPO/ups/setup_for_development -${build_arg} e5 eth

	alias rawEventDump="art -c $artdaq_demo_dir/artdaq-demo/ArtModules/fcl/rawEventDump.fcl"
	alias compressedEventDump="art -c $artdaq_demo_dir/artdaq-demo/ArtModules/fcl/compressedEventDump.fcl"
	alias compressedEventComparison="art -c $artdaq_demo_dir/artdaq-demo/ArtModules/fcl/compressedEventComparison.fcl"
	EOF
    #
fi


echo "Building artdaq-demo..."
cd $ARTDAQDEMO_BUILD
. $demo_dir/setupARTDAQDEMO
buildtool

echo "Installation and build complete; please see https://cdcvs.fnal.gov/redmine/projects/artdaq-demo/wiki/Running_a_sample_artdaq-demo_system for instructions on how to run"

if [ -n "${opt_run_demo-}" ];then
    echo doing the demo

    $artdaq_demo_dir/tools/xt_cmd.sh $demo_dir --geom 132x33 \
        -c '. ./setupARTDAQDEMO' \
        -c start2x2x2System.sh
    sleep 2

    $artdaq_demo_dir/tools/xt_cmd.sh $demo_dir --geom 132 \
        -c '. ./setupARTDAQDEMO' \
        -c ':,sleep 10' \
        -c 'manage2x2x2System.sh -m on init' \
        -c ':,sleep 5' \
        -c 'manage2x2x2System.sh -N 101 start' \
        -c ':,sleep 10' \
        -c 'manage2x2x2System.sh stop' \
        -c ':,sleep 5' \
        -c 'manage2x2x2System.sh shutdown' \
        -c ': For additional commands, see output from: manage2x2x2System.sh --help' \
        -c ':: manage2x2x2System.sh --help' \
        -c ':: manage2x2x2System.sh exit'
fi
