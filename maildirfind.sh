#!/usr/bin/env sh

# 2017-01-07 vixie [holy inbox, batman! that's a lot of files!]

# you know your Maildir layout. you want to use shell commands. you want to
# select filenames by date-range. you're able to cd to the right place. you
# know what you want to do with the results.

# note: it does not call basename, so don't feed it with find(1).

# cd ~/Maildir/cur
# ls | maildirfind.sh 1996 | xargs -P % mv % ~/Maildir/.old.1996/cur/
# -or-
# ls | maildirfind.sh 20161001 20161231 | wc -l
#
# also try:
#
# maildirfind.sh -d 20160701 20160630 < /dev/null

# might have to be explicit about path to get gnu date
DATE_CMD="/opt/local/bin/date"
# might have to be explicit about awk to get gnu awk
AWK_CMD="/opt/local/bin/awk"

#${DATE_CMD}

extract_epochtime() {
	expr "$1" : '\([0-9]*\)\.'
}

#diag=1
if [ "$1" = "-d" ]; then
	diag=1
	shift
else
        diag=0
fi

# actually we want those in YYYY-MM-DD date(1)'s benefit if in YYYYMMDD format

first=`echo $1 | sed 's/^..../&-/;s/^......./&-/'`
last=`echo $2 | sed 's/^..../&-/;s/^......./&-/'`

if [ -n "$first" -a -n "$last" ]; then
	start=$(${DATE_CMD} -d"${first}T00:00:00" +%s)
	finish=$(${DATE_CMD} -d"${last}T11:59:59" +%s)
else
	year="$1"
	if [ -z "$year" ]; then
		echo 'usage: $0 [ yyyy | yyyymmdd yyyymmdd ]' >&2
		exit 1
	fi
	start=$(${DATE_CMD} -d ${year}-01-01T00:00:00 +%s)
	finish=$(${DATE_CMD} -d ${year}-12-31T11:59:59 +%s)
fi

case $diag in
1)	echo S $start -- $(date --date=@${start}) >&2
	echo F $finish -- $(date --date=@${finish}) >&2
	;;
esac

#while read line; do
#	time=$(extract_epochtime $line)
#	case $diag in
#	1)	echo T $line ... $time >&2 ;;
#	esac
#	if [ -z "$time" ]; then
#		continue
#	fi
#	if [ "$time" -ge "$start" -a "$time" -le "$finish" ]; then
#		echo $line
#	fi
#done

awk -v start=${start} -v finish=${finish} -v diag=${diag} '
    { split($0, sl, ".") ; itime=sl[1] }
    (diag == 1) {  printf "T %s ... %d\n", $0, itime > "/dev/stderr" }
    ( itime >= start && itime <= finish ) { print $0 }
'

exit 0
