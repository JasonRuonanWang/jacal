#/bin/bash

# Load common functionality
cmd="cd \$(dirname $0); echo \$PWD; cd \$OLDPWD"
this_dir=`eval "$cmd"`
. $this_dir/common.sh

function print_usage {
	cat <<EOF
$0 [opts]

General options:
 -h/-?                    Show this help and leave
 -V <venv-root>           A virtual environment to load
 -o <output-dir>          The base directory for all outputs

Runtime options:
 -n <nodes>               Number of nodes to use for simulating data, defaults to 1
 -i <num-islands>         Number of data islandsm, defaults to 1
 -c <channels-per-node>   #channels to simulate per node, defaults to 2
 -f <start-freq>          Global start frequency, in Hz. Default=210200000
 -s <freq-step>           Frequency step, in Hz. Default=4000
 -a                       Use the ADIOS2 Storage Manager
 -g                       Use GPUs (one per channel)
 -v <verbosity>           1=INFO (default), 2=DEBUG
 -w <walltime>            Walltime, defaults to 00:30:00
 -M                       Use queue-specific, non-MPI-based daliuge cluster startup mechanism

Runtime paths:
 -b <baseline-exclusion>  The file containing the baseline exclusion map
 -t <telescope-model>     The directory with the telescope model to use
 -S <sky-model>           The sky model to use
EOF
}

# Command line parsing
venv=
outdir=`abspath .`
nodes=1
islands=1 #summit specific
channels_per_node=2
start_freq=210200000
freq_step=4000
baseline_exclusion=
telescope_model=
sky_model=
use_adios2=0
use_gpus=0
#use_gpus=1 #summit specific
verbosity=1
remote_mechanism=mpi
walltime=00:30:00

while getopts "h?V:o:n:c:f:s:b:t:S:agv:w:i:M" opt
do
	case "$opt" in
		h?)
			print_usage
			exit 0
			;;
		V)
			venv="$OPTARG"
			;;
		o)
			outdir="`abspath $OPTARG`"
			;;
		c)
			channels_per_node="$OPTARG"
			;;
		n)
			nodes="$OPTARG"
			;;
		f)
			start_freq=$OPTARG
			;;
		s)
			freq_step=$OPTARG
			;;
		b)
			baseline_exclusion="$OPTARG"
			;;
		t)
			telescope_model="$OPTARG"
			;;
		S)
			sky_model="$OPTARG"
			;;
		a)
			use_adios2=1
			;;
		g)
			use_gpus=1
			;;
		v)
			verbosity=$OPTARG
			;;
		w)
			walltime=$OPTARG
			;;
		i)
			islands=$OPTARG
			;;
		M)
			remote_mechanism=
			;;
		*)
			print_usage 1>&2
			exit 1
			;;
	esac
done
#JACAL_HOME="/gpfs/alpine/csc303/scratch/wangj/jacal" #summit specific
JACAL_HOME="`abspath $this_dir/../..`"
apps_rootdir=$JACAL_HOME"/summit_demo/oskar/ingest"
baseline_exclusion=${baseline_exclusion:-$apps_rootdir/conf/aa2_baselines.csv}
telescope_model=${telescope_model:-$apps_rootdir/conf/aa2.tm}
sky_model=${sky_model:-$apps_rootdir/conf/eor_model_list.csv}

outdir="$outdir/`date -u +%Y-%m-%dT%H-%M-%S`"
mkdir -p "$outdir"

# Turn LG "template" into actual LG for this run
sed "
# Replace filepaths to match our local filepaths
s%\"baseline_exclusion_map_path=.*\"%\"baseline_exclusion_map_path=$baseline_exclusion\"%
s%\"telescope_model_path=.*\"%\"telescope_model_path=$telescope_model\"%
s%\"sky_model_file_path=.*\"%\"sky_model_file_path=$sky_model\"%

# Replace num_of_copies's value in scatter component with $nodes
/.*num_of_copies.*/ {
  N
  N
  s/\"value\": \".*\"/\"value\": \"$nodes\"/
}

# Set whether to use ADIOS2 or not
s/\"use_adios2=.*\"/\"use_adios2=$use_adios2\"/

# Set whether to use GPUs or not
s/\"use_gpus=.*\"/\"use_gpus=$use_gpus\"/
" `abspath $this_dir/graphs/ingest_graph.json` > $outdir"/lg_"$islands"i_"$nodes"n.json"