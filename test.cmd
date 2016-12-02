@echo off
rem Windows
rem Test harness for TraceMiner (Release) which
rem processes all the trace files in the current
rem directory. Assumes that Release/TraceMiner exists.
rem
rem Norman Dunbar
rem 2 December 2016.


for %%x in (*.trc) do (
    echo Testing with trace file: "%%x"
    set DEBUG=%%~nx.dbg
    set OUTPUT=%%~nx.log
    set HTML=%%~nx.html
	
    Release\TraceMiner --html <%%x >%HTML% 2>%DEBUG%
    Release\TraceMiner --verbose <%%x >%OUTPUT% 2>%DEBUG%
)
