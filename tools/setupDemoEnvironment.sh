# this script is intended to be sourced

###########################################
### fetch any user-specified parameters ###
###########################################
# function to print out usage hints
#scriptName=`basename $0`
scriptName="source setupDemoEnvironment.sh"
usage () {
    echo "
Usage: ${scriptName} [options]
Options:
  -h, --help: prints this usage message
  -p <base port number>: specifies the base port number
  -r <boardreader hostname>: specifies a host for a boardreader process
  -e <eventbuilder hostname>: specifies a host for an eventbuilder process
  -o <data directory>: specifies the directory for the output data files
Note: The base port number is used as the starting point for the ports that
      are assigned to the boardreader and eventbuilder processes, and it
      can be specified either by the command-line
      option or the ARTDAQDEMO_BASE_PORT environmental variable.
      It defaults to 5220 if neither are specified.
Note: Multiple boardreader and eventbuilder hosts may be specified.  If
      none are specified, then the current host is used.
Note: The data directory defaults to /tmp if none is specified.
Example: ${scriptName} --baseport 5220
" >&2
}

# parse the command-line options
basePort=""
dataDir=""
let readerHostCount=0
let builderHostCount=0
OPTIND=1
while getopts "hp:r:e:o:-:" opt; do
    if [ "$opt" = "-" ]; then
        opt=$OPTARG
    fi
    case $opt in
        h | help)
            usage
            return 1
            ;;
        p)
            basePort=${OPTARG}
            ;;
        r)
            readerHost[$readerHostCount]=${OPTARG}
            let readerHostCount=${readerHostCount}+1
            ;;
        e)
            builderHost[$builderHostCount]=${OPTARG}
            let builderHostCount=${builderHostCount}+1
            ;;
        o)
            dataDir=${OPTARG}
            ;;
        *)
            usage
            return 1
            ;;
    esac
done
shift $(($OPTIND - 1))

# set a default base port, if needed
if [[ "${basePort}" == "" ]]; then
    if [[ "${ARTDAQDEMO_BASE_PORT}" != "" ]]; then
        basePort=${ARTDAQDEMO_BASE_PORT}
    else
        basePort=5220
    fi
fi

##################################
### set the necessary env vars ###
##################################
# board reader and event builder port numbers
for idx in {0..19}
do
    let ARTDAQDEMO_BR_PORT[idx]=${basePort}+idx
    let ARTDAQDEMO_EB_PORT[idx]=${basePort}+30+idx
done
#export ARTDAQDEMO_BR_PORT
#export ARTDAQDEMO_EB_PORT


# boardreader and event builder host names
for idx in {0..19}
do
    if [[ $idx -lt $readerHostCount ]]; then
        ARTDAQDEMO_BR_HOST[idx]=${readerHost[idx]}
    else
        ARTDAQDEMO_BR_HOST[idx]=`hostname`
    fi
    if [[ $idx -lt $builderHostCount ]]; then
        ARTDAQDEMO_EB_HOST[idx]=${builderHost[idx]}
    else
        ARTDAQDEMO_EB_HOST[idx]=`hostname`
    fi
done
#export ARTDAQDEMO_BR_HOST
#export ARTDAQDEMO_EB_HOST

# data directory
if [[ "${dataDir}" != "" ]]; then
    ARTDAQDEMO_DATA_DIR=$dataDir
else
    ARTDAQDEMO_DATA_DIR=/tmp
fi
export ARTDAQDEMO_DATA_DIR
