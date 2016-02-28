# Test harness for TraceMiner (Release) which
# processes all the trace files in the current
# directory. Assumes that Release/TraceMiner exists.
#
# Norman Dunbar
# 26 February 2016.

for x in `ls *.trc`
do
    echo "Testing with trace file: ${x}"
    DEBUG="${x%%trc}dbg"
    OUTPUT="${x%%trc}out"

    Release/TraceMiner <"${x}" >"${OUTPUT}" 2>"${DEBUG}" --verbose
done
