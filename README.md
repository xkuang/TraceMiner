# Trace Miner

Trace Miner is a utility that scans an Oracle 10046 trace file which has binds included, and for each "dep=0" SQL statement detected - ie Application SQL, will print out the SQL statement with the binds replaced by the actual values used at EXEC time.

The utility reads from stdin, writes its report to stdout, and any other bumff gets written to stderr - so you can keep the desired output separate from the errors, warnings etc. If you run this with the `--verbose` or `-v` option, there _will_ be a _lot_ of text output to the stderr channel. You have been warned. The stderr channel will be populated with the full contents of the trace file plus lots of additional debugging/tracing messages.

Email: <norman@dunbar-it.co.uk>.

## Copyright
Copyright (c) Norman Dunbar, 2016.

## License

There is no license, just use it and abuse it as you see fit. Just leave my copyright messages untouched, that's the only requirement! ;-)

## Execution

`TraceMiner [options] <trace_file.trc [ >report_file.txt ] [ 2>error_file.txt ]`

### Options
  - `--verbose` | `-v`     : Produces lots of output to the stderr channel. Best avoided!
  - `--html` | `-m`        : Anything written to stdout will be in html format.
  - `--help` | `-h` | `-?` : Or any other incorrect option, displays brief help to the stdout channel.

## Compiling
Before compiling, check the section below on configuration. It might prove useful!

Once you have decided on your configuration, it's a simple case of changing into the directory where the various makefiles (`Makefile` and/or `TraceMiner.mak`) are to be found and running the `make` command. When complete there will be an executable file named `TraceMiner` in your current directory.

There are two folders, `Debug` and `Release` with a suitable makefiles named `Makefile` and/or `TraceMiner.mak` in each. The `Release` version is what you should normally be compiling and running but in the event that you encounter errors, the former might be useful if you have a debugger and running in `--verbose` mode didn't/doesn't help!

The following compilation instructions assume that you downloaded the zip file from GitHub, and have unzipped it somewhere:

### Copiling with the Free Embarcadero (Borland) C++ Compiler.

**You will need to have the source code for version 0.16 or higher to compile it with Borland tools.**

If you go to https://www.embarcadero.com/free-tools, you can download the free Borland C++ compiler, version 10.1. Download it somewhere and unzip it. Add the resulting `bcc101\bin` folder to your path, and you are ready to go.

Building is easy:

```
cd TraceMiner-master/Release
build.Borland
```

This will create a 32 bit executable called `TraceMiner.exe` in the `Release` folder. The free compiler generated 32 bit executables.

### Compiling with Visual Studio on Windows
I tried, honest I did! It seems that from version 2005 of Visual Studio, or _probably_ before, the compiler hasn't been updated. It really doesn't like declarations of variables anywhere after lines of code, so all variables have to be declared _at the top_ of the function(s). Quite honestly, I can't be bothered, but I may persevere if I'm ever bored.

Additionally, the GNU function `get_line()` is not available on Windows anyway, although there are versions out  there in the wild that carry out the same/similar functionality that could be used. I have tried this out and I have a version that sort of runs ok, but it randomly aborts at different places in the code for no reason. Running it via the debugger doesn't help as it runs fine in debug mode. Sigh.

I gave up in the end and installed Linux Mint 18 (Sarah) into a VirtualBox VM on my desktop, and used that to compile and run the code on. All I had to do was get my trace files onto the VM - easy using shared folders. Much easier.

### Compiling with Mingw on Windows
This too has problems, which is surprising as it's supposedly GNU compatible. It cannot find the `get_line()` function even though `_GNU_SOURCE` has been defined, which is supposed to make it work. It doesn't so far. More work required to find out why I'm afraid.

I'm actually beginning to think that a Java version on Windows might be the best option.


### Compiling with the Qt Creator IDE on Linux
If you use Qt Creator, simply open the project file `TraceMiner-master\TraceMiner\TraceMiner.pro` and compile in the usual manner after selecting the `Debug` or `Release` build option as desired.


### Compiling with Qt on the Command Line on Linux
You can, if you have Qt installed, use the default `Makefile` to compile the utility from the command line, although you may need to change some paths etc to point at your specific installation of Qt.
```
cd TraceMiner-master/Release
make
```

### Compiling Without Qt
If you have GNU utilities and/or your setup is GNU aware and/or you do not have Qt installed, the following should just work:
```
cd TraceMiner-master/Release
make -f TraceMiner.mak
```
Use TraceMiner32.mak if you are on a 32 bit system.

### When the Makefile Doesn't Work - AIX?
The following assumes you live in _AIX World_ when it complains about GNU stuff. It can also be used if you are stuck in a world that is not GNU aware or if the `TraceMiner.mak` doesn't work with your specific `make` setup:

```
cd TraceMiner-master/Release
/usr/vac/bin/xlc -I../TraceMiner ../TraceMiner/TraceMiner.c ../TraceMiner/utils.c ../TraceMiner/linkedList.c
mv a.out TraceMiner
```
It appears that this particular AIX C compiler (`xlc`) will not create the output file with the name you want it to. Or so I have been reliably informed by a DBA suffering from AIX (_Hi Rich!_). You may need to change the path to the `xlc` compiler to suit your system.

If there is a file named `Makefile` then you may happily ignore it unless you are using QT Creator as your IDE. That file has a lot of dependencies on the QT files for some reason. I'm not sure why this should be, I used the IDE to create a non-QT application.

## Configuration
There are a few options that you can configure _before_ compiling. All are present in the file `config.h` and this file should be edited to suit your system. The options are:

  - MAXCURSORSIZE : Default 20. This defines the length of the text making up a cursor id in a trace file. These used to be numeric, starting from 1 and incrementing, but from 9i, I think, Oracle changed to using a much larger cursor id. On Linux, at least at Oracle version 11.2.0.2, cursor ids are '#140136345356328' and this is 16 characters. On AIX with Oracle 11.2.0.3, a cursor id is '#4574461088' which is only 11 characters. Other systems may differ, so this option defaults to 20 and should be good enough. Change it if you get a message telling you that it needs changing. The message will hint at a suitable value, to save you counting digits!

  - MAXBINDS : Default 50. This defines the maximum number of bind variables that can occur in any one statement. If you see a message telling you to change it, you will be advised of the minimum value that it needs setting to, based on the SQL that broke the current settings though - it there's another SQL further down the trace file with more, then you will get the same error again.

  - MAXBINDSIZE : Default 50. This defines the maximum number of characters that will be output for each bind value when an SQL statement's EXEC line is detected in the trace file. Having this set too low might cause you to miss some important data while having it too high will waste space. Any bind value longer than this setting is simply truncated in the output - it will not stop the utility from running, unlike the two options above.

  - OFFSETFORRICH : Default 128. Should be zero. When testing I used some supplied trace files (Hi Rich) _obviously_ obfuscated for security reasons. This meant that the "len=nnn" section on the `PARSING IN CURSOR` line was untrustworthy as it reported the original size of the _unobfuscated_ SQL statement.  As the `nnn` value is used to allocate a buffer to hold the SQL, the results were sometimes nasty! By adding this offset to the original length, the complete SQL could be extracted and stored.

## Bind Variable Types

  - Data type 1 = VARCHAR2 or NVARCHAR2. Oracle uses the same data type for both. That's a pain! In the code the VARCHAR2 values are wrapped in double quotes, while the NVARCHAR2 values are dumped as hex. The utility copes for this by checking the first character of the value for a double quote, and if _not_ found, pretends that it's really a type 96 (NCHAR) and extracts the value from the hex bytes. Joy!

  - Data type 2 = NUMBER of some kind. These are easy to extract as the value is simply the numeric value passed to the SQL. Sometimes, see below, the value is in the trace file as '###' and a message indicates that an illegal number has been found. This is flagged up by the utility. _I think it might be possible that this isn't really an illegal number, but an OUT parameter for a PL/SQL call. I need to do a lot more testing to be certain._

  - Data Type 11 = ROWID. These are extracted as is from the trace file.

  - Data Type 23 = RAW. These are just a string of hex digits. Currently just extracted as just that, a string of digits.

  - Data Type 25 = Listed in the trace file as unhandled data type. Not much I can do here! It gets output as "Unhandled data type".

  - Data Type 29 = Listed in the trace file as unhandled data type. Not much I can do here! It gets output as "Unhandled data type".

  - Data Type 96 = NCHAR. The value is dumped as hex values in the trace file, but the utility converts those back to a string. It cheats a little (a lot!) as the data are in pairs - "0 31 0 32 0 33" for a string of digits "123". All the utility does is ignore the leading 0 byte and converts the 31, 32 and 33 back into whatever ASCII character they are. So far, so good, but if your use of the utility breaks something, send me the trace file and the details. Ta.

  - Data Type 123 = A buffer. I've seen this type for the buffer to receive the line generated by DBMS_OUTPUT. The trace file has something like `BEGIN DBMS_OUTPUT.GET_LINES(:LINES, :NUM_LINES); END;`. The `NUM_LINES` bind is a plain old NUMBER - data type 2, while the buffer is a data type 123. This is simply converted into some text to indicate that it's a buffer, so the output looks like `BEGIN DBMS_OUTPUT.GET_LINES(:A_BUFFER, ## Illegal number ##); END;`. (See Data Type 2 above for a possible reason why the second parameter is flagged as an illegal number.)

  There are other bind variable data types, but so far, everything I've thrown at the utility has come out the other end quite happily. Famous last words?

__Have Fun.__


