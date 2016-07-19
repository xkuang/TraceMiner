# Trace Miner Updates

### Version 0.13 - 19th july 2016.
  - Tidy up makefiles of (more) spurious stuff.
  - 32 bit Makefile added to Release folder.
  - Deleted the Debug folder. It was still QT dependent for some reason.
  - `MAXBINDS` and `MAXBINDSIZE` removed from `bindvalues.h` - duplicated from `config.h`.

### Version 0.12 - 10th March 2016.
  - [Issue #1](https://github.com/NormanDunbar/TraceMiner/issues/1): Spurious date/time stamps in trace file cause utility to barf! Fixed.
  - Output format changed to identify EXEC and PARSE lines.
  - HTML output also amended as above.

### Version 0.11 - 29th February 2016.
  - A couple of (internal) bug fixes.
  - HTML output added.
  - `OFFSETFORRICH` setting moved to `config.h` for ease of adjustment.

### Version 0.1 - 26th February 2016.
This was the first release. 
