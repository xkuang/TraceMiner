:: Assumes that your Borland compiler (v10.1 in my case) is on %PATH%
:: eg set PATH=c:\borland\bcc101\bin;%PATH%
::
:: Builds into the current folder.
::

bcc32c -o TraceMiner.exe ..\TraceMiner\*.c
if exist "TraceMiner.tds" (
    @del "TraceMiner.tds" 2 > nul
)

pause