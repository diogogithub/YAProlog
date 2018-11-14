 YAP 6-3.4 Manual                         {#mainpage}
====================

This file documents the YAP Prolog System version 6.3.4, a high-performance Prolog compiler developed at LIACC, Universidade do Porto. YAP is based on David H. D. Warren's WAM (Warren Abstract Machine), with several optimizations for better performance. YAP follows the Edinburgh tradition, and is largely compatible with DEC-10 Prolog, Quintus Prolog, and especially with C-Prolog.

+ @ref  download

+ @ref  install

+ @ref run

+ @ref YAPSyntax

+ @ref consult

+ @ref builtins

+ @ref extensions

+ @ref library

+ @ref packages

+ @ref swi

+ @ref YAPProgramming

+ @ref fli



\author Vitor Santos Costa,
\author Luís Damas,
\author Rogério Reis
\author Rúben Azevedo


© 1989-201 L. Damas, V. Santos Costa and Universidade
do Porto.
Permission is granted to make and distribute verbatim copies of this manual provided the copyright notice and this permission notice are preserved on all copies.
Permission is granted to copy and distribute modified versions of this manual under the conditions for verbatim copying, provided that the entire resulting derived work is distributed under the terms of a permission notice identical to this one.
Permission is granted to copy and distribute translations of this manual into another language, under the above conditions for modified versions.

<center>
![The YAP Logo](yap_256x256x32.png)
</center>

This file contains extracts of the SWI-Prolog manual, as written by Jan
Wielemaker. Our thanks to the author for his kind permission in allowing
us to include his text in this document.


## Introduction

This document provides User information on version 6.3.4 of
YAP (<em>Yet Another Prolog</em>). The YAP Prolog System is a
high-performance Prolog compiler developed at Universidade do
Porto.  YAP supports stream Input/Output, sockets, modules,
  exceptions, Prolog debugger, C-interface, dynamic code, internal
  database, DCGs, saved states, co-routining, arrays, threads.

We explicitly allow both commercial and non-commercial use of YAP.


YAP is based on the David H. D. Warren's WAM (Warren Abstract Machine),
with several optimizations for better performance. YAP follows the
Edinburgh tradition, and was originally designed to be largely
compatible with DEC-10 Prolog, Quintus Prolog, and especially with
C-Prolog. More recently, we have worked on being compatible with SICStus Prolog and with SWI-Prolog.

YAP implements most of the ISO-Prolog standard. We are striving at
full compatibility, and the manual describes what is still
missing.
The document is intended neither as an introduction to Prolog nor to the
implementation aspects of the compiler. A good introduction to
programming in Prolog is the book @cite TheArtOfProlog , by
L. Sterling and E. Shapiro, published by "The MIT Press, Cambridge
MA". Other references should include the classical @cite ProgrammingInProlog , by W.F. Clocksin and C.S. Mellish, published by
Springer-Verlag.

YAP 6.3.4 has been built with the gcc and clang compilers on Linux and OSX machines. We expect to recover support for  WIN32 machines and
Android next.

The overall copyright and permission notice for YAP4.3 can be found in
the Artistic file in this directory. YAP follows the Perl Artistic
license, and it is thus non-copylefted freeware. Some components of YAP have been obtained from SWI Prolog and ciao, and have
different ;icenses.

If you have a question about this software, desire to add code, found a
bug, want to request a feature, or wonder how to get further assistance,
please send e-mail to <yap-users AT lists.sourceforge.net>.  To
subscribe to the mailing list, visit the page
<https://lists.sourceforge.net/lists/listinfo/yap-users>.

On-line documentation is available for YAP at:

<http://www.ncc.up.pt/~vsc/yAP/>

Recent versions of YAP, including both source and selected binaries,
can be found from this same URL.

This manual was written by Vítor Santos Costa,
Luís Damas, Rogério Reis, and Rúben Azevedo. The
manual is largely based on the DECsystem-10 Prolog User's Manual by
D.L. Bowen, L. Byrd, F. C. N. Pereira, L. M. Pereira, and
D. H. D. Warren. We have  used comments from the Edinburgh Prolog
library written by R. O'Keefe. Documentation from many built-ins is
originally from the SWI-Prolog manual, with the gracious authorization
from
Jan Wielemaker. We would also like to gratefully
acknowledge the contributions from Ashwin Srinivasian.

We are happy to include in YAP several excellent packages developed
under separate licenses. Our thanks to the authors for their kind
authorization to include these packages.

The packages are, in alphabetical order:

+ The CHR package developed by Tom Schrijvers,
Christian Holzbaur, and Jan Wielemaker.

+ The CLP(BN) package and Horus toolkit developed by Tiago Gomes, and Vítor Santos Costa.

+ The CLP(R) package developed by Leslie De Koninck, Bart Demoen, Tom
Schrijvers, and Jan Wielemaker, based on the CLP(Q,R) implementation
by Christian Holzbaur.

+ The CPLint package developed by Fabrizio Riguzzi's research
laboratory at the University of Ferrara. Please see

<http://www.ing.unife.it/Docenti/FabrizioRiguzzi/>

+ The CUDA interface package developed by Carlos Martínez, Jorge
Buenabad, Inês Dutra and Vítor Santos Costa.

+ The GECODE interface package developed by  Denys Duchier and Vítor Santos Costa.

+ The JPL (Java-Prolog Library) package developed by .

 The minisat SAT solver interface developed by Michael Codish,
 Vitaly Lagoon, and Peter J. Stuckey.

+ The MYDDAS relational data-base interface developed at the
 Universidade do Porto by Tiago Soares, Michel Ferreira, and Ricardo Rocha.

+ The PRISM logic-based
programming system for statistical modeling developed at the Sato
Research Laboratory, TITECH, Japan.

+ The ProbLog 1 system developed by the ProbLog team in the
DTAI group of KULeuven. For general information on ProbLog 1 and 2, please see

    <http://dtai.cs.kuleuven.be/problog>

+ The `real` R interface package developed by 	Nicos Angelopoulos,
Vítor Santos Costa, João Azevedo, Jan Wielemaker, and Rui Camacho.

+ YAP includes the `yap2swi` library that ports to YAP code from
 of SWI's PL interface. This includes the Input/Output Layer, the SWI
 Foreign Language Interface, and the RDF, archive, clib, http, odbc, plunit,
 semweb, sgml, and zlib packages written by Jan Wielemaker.

Downloading YAP           {#download}
==============

The latest development version of Yap-6 is yap-6.3.4 and can be
obtained from the repositories

<http://sourceforge.net/p/yap/yap-6.3>

and

<https://github.com/vscosta/yap-6.3>

YAP-6.3.4 does not use repositories. Please just use `git clone` to obtain the distribution.

Most of these repositories are basically copies of the original
repositories at the SWI-Prolog site. YAP-6 will work either with or
without these packages.

Installing YAP           {#install}
==============

YAP is a `ccmake` based system. We discuss how to use `cmake`
to install YAP, and what are the major options.

Compiling YAP {#CompilingYAP}
-------------

To compile YAP it should be sufficient to:

2 create a directory, say `Build`.

  obs: please avoid compiling YAP in the src directory, some packages do not allow for that.

1 run `cmake`, ideally using a cmake above 3.0.

2 `make`.

3 If the compilation succeeds, try `./yap`.

4  If you feel satisfied with the result, do `make install`.

5 In most systems you will need to be superuser in order to do
    `make install` and `make info` on the standard directories.

Tuning the Functionality of YAP
-------------------------------

By default, YAP supports tabling, depth first search, and most features found in
modern Prologs. In some cases, you may want to suport extra features, or reduce system size.
`cmake`provides an graphical interface for doing so. From the commmand line,  a typical example could be:

~~~~~
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~
~~~~~

Options include:

+ `CMAKE_BUILD_TYPE`: the two main options are Debug, for system development, and Release. to use the system.

+ `CMAKE_INSTALL_PREFIX`: where to install YAP

+ WITH_DEPTH_LIMIT allows depth limited evaluation, say for
implementing iterative deepening (default)

+ `WITH_CALL_TRACER` allows support for tracing all calls,
retries, and backtracks in the system. This can help in debugging your
application, but results in performance loss (enabled in Debug).

  + `WITH_WAM_PROFILER` allows profiling of abstract machine
instructions. This is useful when developing YAP, should not be so
useful for normal users (not currently supportted)

  + `WITH_YAP_CONDOR` allows using the Condor system that
support High Throughput Computing (HTC) on large collections of
distributively owned computing resources (not yet available)

  + `WITH_TABLING` allows tabling support (default)

  + `WITH_YAPOR_COPY` allows
or-parallelism according to the Muse-based, YapOR model. This option is
still highly experimental.

  /*+ `--with-max-workers` allows definition of the maximum
number of parallel processes (its value can be consulted at runtime
using the flag `max_workers`).*/

+ `WITH_GMP` give a path to where one can find the
`GMP` library if not installed in the default path. It is highly advised to have GMP in your compilation environment.

+ `WITH_READLINE` give a path to where one can find the
`readline` library if not installed in the default path. It is highly advised in Unix-like environments.

 + `-WITH_Threads` allows using of the multi-threading
predicates provided by YAP. Depending on the operating system, the
option `--enable-pthread-locking` may also need to be used.

  /*+ `--with-max-threads` allows definition of the maximum
number of threads (the default value is 1024; its value can be consulted
at runtime using the flag [max_threads](@ref max_threads)).*/



Next section discusses machine dependent details, and it is out-of-date.

Tuning YAP for a Particular Machine and Compiler {#Machine_Options}
------------------------------------------------

The default options should give you best performance under
`GCC`. Although the system is tuned for this compiler
we have been able to compile versions of YAP under lcc in Linux,
Sun's cc compiler, IBM's xlc, SGI's cc, and Microsoft's Visual C++
6.0.

###  Tuning YAP for `GCC`. {#Tuning_for_GCC}

YAP has been developed to take advantage of `GCC` (but not to
depend on it). The major advantage of `GCC` is threaded code and
explicit register reservation.

YAP is set by default to compile with the best compilation flags we
know. Even so, a few specific options reduce portability.  The option

  + `--enable-max-performance=yes` will try to support the best
available flags for a specific architectural model. Currently, the option
assumes a recent version of `GCC`.
  + `--enable-debug-yap` compiles YAP so that it can be debugged
by tools such as `dbx` or `gdb`.


Here follow a few hints:

On x86 machines the flags:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YAP_EXTRAS= ... -DBP_FREE=1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

tells us to use the `%bp` register (frame-pointer) as the emulator's
program counter. This seems to be stable and is now default.

On  Sparc/Solaris2 use:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YAP_EXTRAS= ...   -mno-app-regs -DOPTIMISE_ALL_REGS_FOR_SPARC=1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

and YAP will get two extra registers! This trick does not work on
SunOS 4 machines.

Note that versions of GCC can be tweaked to recognize different
processors within the same instruction set, e.g. 486, Pentium, and
PentiumPro for the x86; or Ultrasparc, and Supersparc for
Sparc. Unfortunately, some of these tweaks do may make YAP run slower or
not at all in other machines with the same instruction set, so they
cannot be made default.

Last, the best options also depends on the version of GCC you are using, and
it is a good idea to consult the GCC manual under the menus "Invoking
GCC"/"Submodel Options". Specifically, you should check
`-march=XXX` for recent versions of GCC/EGCS. In the case of
`GCC2.7` and other recent versions of `GCC` you can check:

  + 486:
In order to take advantage of 486 specific optimizations in GCC 2.7.\*:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YAP_EXTRAS= ... -m486 -DBP_FREE=1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  + Pentium:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YAP_EXTRAS= ... -m486 -malign-loops=2 -malign-jumps=2 \
                      -malign-functions=2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  + PentiumPro and other recent Intel and AMD machines:
PentiumPros are known not to require alignment. Check your version of
`GCC` for the best `-march` option.

  + Super and UltraSparcs:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YAP_EXTRAS= ... -msupersparc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  + MIPS: if have a recent machine and you need a 64 bit wide address
space you can use the abi 64 bits or eabi option, as in:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CC="gcc -mabi=64" ./configure --...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Be careful. At least for some versions of `GCC`, compiling with
`-g` seems to result in broken code.

### Compiling Under MINGW's GCC {#Compiling_under_mingw}


AT the time of this writing (Nov 2014), YAP uses the mkwin script to
compile in WIN32. The script requires either a WIN32 environment, or a
cross-compiler/emulator package.

YAP has been known to compile under VISUAL C++, and should compile and
work under cygwin, but the favorite approach is to use a native
msys/mingw environment. This approach has two key advantages:

  + it does not need an interface layer and a DLL, like cygwin.

  + it enables cross-compilation.

YAP uses rge `mkwin` script to generate a new YAP installer. The script is
controlled by a set of of variables that should be defined early on in
the text. It executes by first calling `configure`, next running `make`, and
last (if all went well) executing `nsys`.

In more detail, the following mingw based environments have been
tested to develop YAP:

  * MSYS 1 and mingw32/64: most WIN32 development did occur in this
    native environment. Best results were achieved with
    MSYS-1.0.* and TDM-GCC:

    mingw: http://www.mingw.org/
    original msys: http://www.mingw.org/wiki/MSYS
    mingw64: http://mingw-w64.sourceforge.net/
    TDM-GCC: http://tdm-gcc.tdragon.net/

  * This distribution was compiled with the MSYS2 integrated
    development, that supports 32 and 64 bit compilation. Setting up
    MSYS2 should be done with care, but it is worth it as the
    distribution works nicely in MINGW32 and MINGW64 mode. A third
    compilation mode, MSYS mode, has problems with compiling sockets.

    msys2: http://sourceforge.net/projects/msys2/

  * cygwin and cygwin64 now can generate native applications

    cygwin: https://www.cygwin.com/

  * Linux has a nice cross-compilation environment, with some of the best
    work done for Fedora.

    fedora mingw cross-compiler: http://fedoraproject.org/wiki/MinGW/CrossCompilerFramework

    One problem is that this environment requires emulation of WIN32
    executables to generate the initial saved state and to compile
    `chr`. `wine` sometimes does the task, but it sometimes fails.

  * OSX has the `mxe` package, a port of mingw that is in active
    development.

    mxe: http://mxe.cc/

    Note that OSX has technical limitations that preclude porting
    wine64. wine32 is distributed with package managers such as ports
    and brew.

### Setting up WIN32 compilation

Compiling WIN32 packages depends on a number of parameters: chosen compiler,
packages to install, directory setup. You may have to change these ones that
control the `mkwin` script:

 * `VER`: major/minor number
 * `PATCHID`: third digit
 * `SRC`: directory containing yap sources, in the local environment notation.
 * `SRC_WIN`: same, but in WIN32 standard notation.
 * `THREADS`: yes or no? controllable from the command line.
 * `ABI`: "32" or "64", controllable from the command line.
 * `NSIS`: installer generator, usually "/c/Program Files (x86)/NSIS/makensis".
 * `DOCS_DIR`: where you have the doxygen output.
 * `GCC_DIR`: root of gcc seup.
 * `HOST`: argument to `--host` configure command.
 * `BUILD`: build directory
 * `GMP`: multi-precision package; yes, no, or the installation directory; usually in the distribution.
 * `CUDD`: BDD package, usually in the distribution.
 * `JAVA`: Java sdk directory, usually in the distribution.
 * `PYTHON`: Python package, usually in the distribution.
 * `R`: R environment package, usually in the distribution.
 * `GECODE`: constraint solver package, usually not in the WIN32 distribution.

###  Compiling Under Visual C++ {#Compiling_Under_Visual_C}

YAP used to compile cleanly under Microsoft's Visual C++ release 6.0. We next
give a step-by-step review on how the core YAP compiled manually using this
environment. Newer versions of YAP will use cmake for this purpose.

First, it is a good idea to build YAP as a DLL:

  + create a project named yapdll using File.New. The project will be a
DLL project, initially empty.

Notice that either the project is named yapdll or you must replace the
preprocessors variable  _$YAPDLL_EXPORTS_ to match your project names
in the files YAPInterface.h and `c_interface.c`.

  + add all .c files in the $YAPSRC/C directory and in the
$YAPSRC\\OPTYAP directory to the Project's `Source Files` (use
FileView).

  + add all .h files in the  _$YAPSRC/H_ directory,
 _$YAPSRC\\include_ directory and in the  _$YAPSRC\\OPTYAP_
subdirectory to the Project's `Header Files`.

  + Ideally, you should now use `m4` to generate extra  .h from .m4 files and use
`configure` to create a `config.h`. Or, you can be lazy, and
fetch these files from  _$YAPSRC\\VC\\include_.

  + You may want to go to `Build.Set Active Configuration` and
set `Project Type` to `Release`

  + To use YAP's own include directories you have to set the Project
option  `Project.Project Settings.C/C++.Preprocessor.Additional Include Directories` to include the directories  _$YAPSRC\\H_,
 _$YAPSRC\\VC\\include_,  _$YAPSRC\\OPTYAP_ and
 _$YAPSRC\\include_.  The syntax is:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$YAPSRC\H, $YAPSRC\VC\include, $YAPSRC\OPTYAP, $YAPSRC\include
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  + Build: the system should generate an yapdll.dll and an yapdll.lib.

  + Copy the file yapdll.dll to your path. The file
    yapdll.lib should also be copied to a location where the linker can find it.


Now you are ready to create a console interface for YAP:

<ol>
 <li>create a second project  say `wyap` with `File.New`. The project will be a
WIN32 console project, initially empty.

  + add  _$YAPSRC\\console\\yap.c_ to the `Source Files`.

  + add  _$YAPSRC\\VC\\include\\config.h_ and the files in  _$YAPSRC\\include_ to
the `Header Files`.

  + You may want to go to `Build.Set Active Configuration` and set
`Project Type` to `Release`.

  + you will eventually need to bootstrap the system by booting from
`boot.yap`, so write:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        -b $YAPSRC\pl\boot.yap
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

in `Project.Project Settings.Debug.Program Arguments`.

  + You need the sockets and yap libraries. Add

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ws2_32.lib yapdll.lib
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

to `Project.Project Settings.Link.Object/Library Modules`

You may also need to set the `Link Path` so that VC++ will find `yapdll.lib`.

  + set `Project.Project Settings.C/C++.Preprocessor.Additional Include Directories` to include the
 _$YAPSRC/VC/include_ and
 _$YAPSRC/include_.

The syntax is:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$YAPSRC\VC\include, $YAPSRC\include
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  + Build the system.

  + Use `Build.Start Debug` to boot the system, and then create the saved state with

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
['$YAPSRC\\pl\\init'].
qsave_program('startup.yss').
^Z
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

That's it, you've got YAP and the saved state!
</li>
</ol>

Loading and Organising YAP Programs      {#consult}
===================================

  @ingroup main

  Next, we present the main predicates and directives available to load
  files and to control the Prolog environment.

  + @ref YAPConsulting

  + @ref YAPModules

  +@ref YAPSaving


  This chapter describes the predicates  controlling the execution of
  Prolog programs.

  In the description of the arguments of functors the following notation
  will be used:

  + a preceding plus sign will denote an argument as an "input
  argument" - it cannot be a free variable at the time of the call;
  + a preceding minus sign will denote an "output argument";
  + an argument with no preceding symbol can be used in both ways.

Running YAP                      {#run}
===========

We next describe how to invoke YAP in Unix systems.

Running YAP Interactively
-------------------------

Most often you will want to use YAP in interactive mode. Assuming that
YAP is in the user's search path, the top-level can be invoked under
Unix with the following command:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
yap [-s n] [-h n] [-a n] [-c IP_HOST port ] [filename]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All the arguments and flags are optional and have the following meaning:

+ -?
print a short error message.
+ -s _Size_
allocate  _Size_ KBytes for local and global stacks. The user may
specify <tt>M</tt> bytes.
+ -h _Size_
allocate  _Size_ KBytes for heap and auxiliary stacks
+ -t _Size_
allocate  _Size_ KBytes for the trail stack
+ -L _Size_
SWI-compatible option to allocate  _Size_ K bytes for local and global stacks, the local stack
cannot be expanded. To avoid confusion with the load option,  _Size_
must immediately follow the letter `L`.
+ -G _Size_
SWI-compatible option to allocate  _Size_ K bytes for local and global stacks; the global
stack cannot be expanded
+ -T _Size_
SWI-compatible option to allocate  _Size_ K bytes for the trail stack; the trail cannot be expanded.
+ -l  _YAP_FILE_
compile the Prolog file  _YAP_FILE_ before entering the top-level.
+ -L  _YAP_FILE_
compile the Prolog file  _YAP_FILE_ and then halt. This option is
useful for implementing scripts.
+ -g  _Goal_
run the goal  _Goal_ before top-level. The goal is converted from
an atom to a Prolog term.
+ -z  _Goal_
run the goal  _Goal_ as top-level. The goal is converted from
an atom to a Prolog term.
+ -b  _BOOT_FILE_
boot code is in Prolog file  _BOOT_FILE_. The filename must define
the predicate `'$live'/0`.
+ -c <tt>IP_HOST</tt> <tt>port</tt>
connect standard streams to host <tt>IP_HOST</tt> at port <tt>port</tt>
+ filename
restore state saved in the given file
+ -f
do not consult initial files
+ -q
do not print informational messages
+ --
separator for arguments to Prolog code. These arguments are visible
through the unix/1 built-in predicate.


Note that YAP will output an error message on the following conditions:

+
a file name was given but the file does not exist or is not a saved
YAP state;

+
the necessary amount of memory could not be allocated;

+
the allocated memory is not enough to restore the state.


    When restoring a saved state, YAP will allocate the
same amount of memory as that in use when the state was saved, unless a
different amount is specified by flags in the command line. By default,
YAP restores the file startup.yss from the current directory or from
the YAP library.

+
YAP usually boots from a saved state. The saved state will use the default
installation directory to search for the YAP binary unless you define
the environment variable YAPBINDIR.

+
YAP always tries to find saved states from the current directory
	first. If it cannot it will use the environment variable YAPLIBDIR, if
	defined, or search the default library directory.

+
YAP will try to find library files from the YAPSHAREDIR/library
directory.


Prolog Scripts
--------------

YAP can also be used to run Prolog files as scripts, at least in
Unix-like environments. A simple example is shown next (do not forget
that the shell comments are very important):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#!/usr/local/bin/yap -L --
#
# Hello World script file using YAP
#
# put a dot because of syntax errors .

:- write('Hello World'), nl.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The `#!`  characters specify that the script should call the binary
file YAP. Notice that many systems will require the complete path to the
YAP binary. The `-L` flag indicates that YAP should consult the
current file when booting and then halt. The remaining arguments are
then passed to YAP. Note that YAP will skip the first lines if they
start with `#` (the comment sign for Unix's shell). YAP will
consult the file and execute any commands.

A slightly more sophisticated example is:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#!/usr/bin/yap -L --
#
# Hello World script file using YAP
# .

:- initialization(main).

main :- write('Hello World'), nl.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The `initialization` directive tells YAP to execute the goal main
after consulting the file. Source code is thus compiled and `main`
executed at the end. The `.` is useful while debugging the script
as a Prolog program: it guarantees that the syntax error will not
propagate to the Prolog code.

Notice that the `--` is required so that the shell passes the extra
arguments to YAP.  As an example, consider the following script
`dump_args`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#!/usr/bin/yap -L --
#.

main( [] ).
main( [H|T] ) :-
        write( H ), nl,
        main( T ).

:- unix( argv(AllArgs) ), main( AllArgs ).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you this run this script with the arguments:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
./dump_args -s 10000
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
the script will start an YAP process with stack size `10MB`, and
the list of arguments to the process will be empty.

Often one wants to run the script as any other program, and for this it
is convenient to ignore arguments to YAP. This is possible by using
`L --` as in the next version of `dump_args`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#!/usr/bin/yap -L --

main( [] ).
main( [H|T] ) :-
        write( H ), nl,
        main( T ).

:- unix( argv(AllArgs) ), main( AllArgs ).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The `--` indicates the next arguments are not for YAP. Instead,
they must be sent directly to the argv built-in. Hence, running

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
./dump_args test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

will write `test` on the standard output.

YAP Built-ins          {#builtins}
=============

  + @ref YAPControl

  + @ref arithmetic

  + @ref YAPChars

  + @ref YAP_Terms

  + @ref InputOutput
  
  + @ref AbsoluteFileName

  + @ref YAPOS

  + @ref Internal_Database

  + @ref Sets

Extensions to core Prolog.  {#extensions}
==========================

YAP includes a number of extensions over the original Prolog
language. Next, we discuss how to use the most important ones.

  + @ref Rational_Trees

  + @ref AttributedVariables

  + @ref  DepthLimited

  + @ref  Tabling

  + @ref Threads

  + @ref Profiling

  + @ref YAPArrays

  + @ref Parallelism

The YAP Library        {#library}
===============

@defgroup library YAP library files
@{

  Library files reside in the library_directory path (set by the
  `LIBDIR` variable in the Makefile for YAP). Several files in the
  library are originally from the public-domain Edinburgh Prolog library.

- @ref apply
- @ref apply_macros
- @ref arg
- @ref Association_Lists
- @ref avl
- @ref bhash
- @ref block_diagram
- @ref c_alarms
- @ref charsio
- @ref clauses
- @ref cleanup
- @ref dbqueues
- @ref dbusage
- @ref dgraphs
- @ref exo_interval
- @ref flags
- @ref gensym
- @ref yap_hacks
- @ref heaps
- @ref lam_mpi
- @ref line_utils
- @ref swi_listing
- @ref lists
- @ref mapargs
- @ref maplist
- @ref matlab
- @ref matrix
- @ref nb
- @ref Ordered_Sets
- @ref parameters
- @ref queues
- @ref random
- @ref Pseudo_Random
- @ref rbtrees
- @ref regexp
- @ref rltrees
- @ref Splay_Trees
- @ref operating_system_support,
- @ref Terms
- @ref timeout
- @ref trees
- @ref tries
- @ref ugraphs
- @ref undgraphs
- @ref varnumbers
- @ref wdgraphs
- @ref wdgraphs
- @ref wdgraphs
- @ref wgraphs
- @ref wundgraphs
- @ref ypp
@}

The YAP Packages  {#packages}
================

@defgroup packages YAP packages files
@{

+ @ref real

+ @ref BDDs

+ @ref  Gecode

+ @ref  MYDDAS

+ @ref PFL

+ @ref ProbLog1

+ @ref python

+ @ref YAPRaptor

+ @ref YAP-LBFGS

+ @subpage yap-udi-indexers

Leuven packages ported from SWI-Prolog:

+ @subpage chr

+ @subpage clpqr

@}

Compatibility {#swi}
=============


@defgroup swi  Compatibility 
@{


YAP has been designed to be as compatible as possible with other
Prolog systems, originally with C-Prolog\cite x and SICStus
Prolog~\cite x . More recent work on YAP has striven at making YAP
compatible with the ISO-Prolog standard\cite x , and with Jan
Wielemaker's SWI-Prolog\cite x .

SWI-Prolog and YAP have collaborated at improved compatibility \cite x . This
resulted in Prolog extensions such as the `dialect` feature. YAP
currently supports most of the SWI-Prolog foreign interface. The following  SWI
libraries have worked on YAP:

  + @ref aggregate
  + @ref base64
  + @ref broadcast
  + @ref ctypes
  + @ref date
  + @ref prolog_debug
  + @ref prolog_edit
  + @ref error
  + @ref nb_set
  + @ref prolog_operator
  + @ref swi_option
  + @ref pairs
  + @ref pio
  + @ref predicate_options,
  + @ref predopts
  + @ref prolog_clause
  + @ref prolog_colour
  + @ref prolog_source
  + @ref prolog_xref
  + @ref pure_input
  + @ref quasi_quotations
  + @ref read_util
  + @ref record
  + @ref settings
  + @ref shlib
  + @ref thread_pool
  + @ref url
  + @ref utf8
  + @ref win_menu
  + @ref www_browser


Note that in general SWI code may be from an earlier version than the
one available with SWI-Prolog. SWI-Prolog are obviously not
responsible for any incompatibilities and/or bugs in the YAP port.

Please do refer to the SWI-Prolog home page:

<http://www.swi-prolog.org>

for more information on SWI-Prolog and the SWI packages.

Compatibility with the C-Prolog interpreter {#ChYProlog}
-------------------------------------------

YAP was designed so that most C-Prolog programs should run under YAP
without changes.
The most important difference between YAP and C-Prolog is that, being
YAP a compiler, some changes should be made if predicates such as
assert/1, clause/1 and retract/1 are used. First
predicates which will change during execution should be declared as
`dynamic` by using commands like:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
:- dynamic f/n.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 where `f` is the predicate name and n is the arity of the
predicate. Note that  several such predicates can be declared in a
single command:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 :- dynamic f/2, ..., g/1.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Primitive predicates such as `retract` apply only to dynamic
predicates.  Finally note that not all the C-Prolog primitive predicates
are implemented in YAP. They can easily be detected using the
`unknown` system predicate provided by YAP.

Last, by default YAP enables character escapes in strings. You can
disable the special interpretation for the escape character by using:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
:- yap_flag(character_escapes,off).
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
or by using:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
:- yap_flag(language,cprolog).
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Compatibility with the Quintus and SICStus Prolog systems
---------------------------------------------------------

The Quintus Prolog system was the first Prolog compiler to use Warren's
Abstract Machine. This system was very influential in the Prolog
community. Quintus Prolog implemented compilation into an abstract
machine code, which was then emulated. Quintus Prolog also included
several new built-ins, an extensive library, and in later releases a
garbage collector. The SICStus Prolog system, developed at SICS (Swedish
Institute of Computer Science), is an emulator based Prolog system
largely compatible with Quintus Prolog. SICStus Prolog has evolved
through several versions. The current version includes several
extensions, such as an object implementation, co-routining, and
constraints.

Both YAP and SICStus Prolog obey the Edinburgh Syntax and are based on
the WAM. Even so, there are major important differences:

  + Differently from SICStus Prolog, both consulted and dynamic code in YAP
  are compiled, not interpreted. All code in YAP is compiled.

  + The following SICStus Prolog v3 built-ins are not (currently)
implemented in YAP (note that this is only a partial list):
stream_interrupt/3, reinitialize/0, help/0, help/1,
trimcore/0, and require/1.

  + The consult/1 predicate in YAP follows C-Prolog
semantics. That is, it adds clauses to the data base, even for
preexisting procedures. This is different from consult/1 in
SICStus Prolog or SWI-Prolog.

  + This list is incomplete.

Compatibility with the ISO Prolog standard
------------------------------------------

The Prolog standard was developed by ISO/IEC JTC1/SC22/WG17, the
international standardization working group for the programming language
Prolog. The book "Prolog: The Standard" by Deransart, Ed-Dbali and
Cervoni gives a complete description of this standard. Development in
YAP from YAP4.1.6 onwards have striven at making YAP
compatible with ISO Prolog. As such:

  + YAP now supports all of the built-ins required by the
ISO-standard, and,
  + Error-handling is as required by the standard.


YAP by default is not fully ISO standard compliant. You can set the
language flag to `iso` to obtain better
compatibility. Setting this flag changes the following:


  + By default, YAP implements the
atom_chars/2 (see Testing Terms), and
number_chars/2,  (see Testing Terms),
built-ins as per the original Quintus Prolog definition, and
not as per the ISO definition.

Calling `set_prolog_flag(to_chars_mode,iso)` will switch
YAP to use the ISO definition for
atom_chars/2 and number_chars/2.

  + By default, YAP allows executable goals in directives. In ISO mode
most directives can only be called from top level (the exceptions are
set_prolog_flag/2 and op/3).

  + Error checking for meta-calls under ISO Prolog mode is stricter
than by default.

  + The strict_iso flag automatically enables the ISO Prolog
standard. This feature should disable all features not present in the
standard.

The following incompatibilities between YAP and the ISO standard are
known to still exist (please check Ulrich Neumerkel's page for more details):

<ul>

 <li>Currently, YAP does not handle overflow errors in integer
operations, and handles floating-point errors only in some
architectures. Otherwise, YAP follows IEEE arithmetic.

Please inform the authors on other incompatibilities that may still
exist.

@}

Foreign Language interface for YAP     {#fli}
==================================

@defgroup fli Foreigd Code Interfacing

@{

YAP provides the user with three facilities for writing
predicates in a language other than Prolog. Under Unix systems,
most language implementations were linkable to `C`, and the first interface exported  the YAP machinery to the C language. YAP also implements most of the SWI-Prolog foreign language interface.
This gives portability with a number of SWI-Prolog packages and avoids garnage collection by using @ref slotInterface. Last, a new C++ based interface is
being designed to work with the swig (www.swig.orgv) interface compiler.

+ The @ref c-interface exports the YAP engine.

+ The @ref swi-c-interface emulates Jan Wielemaker's SWI foreign language interface.

+ The @ref  yap-cplus-interface is desiged to interface with the SWIG package by using Object-Oriented concepts

+ The @ref LoadInterface handles the setup of foreign files

@}

