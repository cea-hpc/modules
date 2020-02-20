#
# This should be sourced from the other scripts

# Make sure TMP_MODVERSION and TMP_STRATEGY are set
if [ "x$TMP_MODVERSION" = "x" ]; then
	echo >&2 "ERROR: TMP_MODVERSION not set"
	exit
fi

if [ "x$TMP_STRATEGY" = "x" ]; then
	echo >&2 "ERROR: TMP_STRATEGY not set"
	exit
fi

# Set prompt
TMP_PROMPT="mod${TMP_MODVERSION}"
case $TMP_STRATEGY in
   flavours)
	#This really means MODVERSION is flavours version
	TMP_PROMPT="${TMP_PROMPT}-flavours"
	;;
   homebrewed|modulerc|modulepath)
	TMP_PROMPT="${TMP_PROMPT} ($TMP_STRATEGY)"
	;;
   *)
	echo >&2 "ERROR: unrecognized value '$TMP_STRATEGY' for TMP_STRATEGY"
	exit 1
	;;
esac

#Define do_cmd function
do_cmd()
{	cmd="$@"
	echo "[$TMP_PROMPT]$ $cmd"
	$cmd
}

#Define variables GCCGNU to be gcc or gnu
case $TMP_STRATEGY in
   flavours)
	GCCGNU=gnu
	;;
   *)
	GCCGNU=gcc
	;;
esac

#Define AUTOFLAG to be "" or --auto
case $TMP_MODVERSION in
   3)
	AUTOFLAG=
	;;
   4)
	case $TMP_STRATEGY in
	   flavours)
		AUTOFLAG=
		;;
	   *)
		AUTOFLAG="--auto"
		;;
	esac
	;;
esac

