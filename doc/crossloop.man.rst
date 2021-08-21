=========
crossloop
=========

-------------------------------------------
restart crossfire-server in case of a crash
-------------------------------------------

:Date: August 20, 2021
:Manual group: Crossfire Server Manual
:Manual section: 6

SYNOPSIS
========
**crossloop**

DESCRIPTION
===========
**crossloop** launches **crossfire-server** in a loop, restarting it in case
of a crash. If configured to do so, **crossloop** will send an email with
information about the crash that can be useful in making a bug report.

**crossloop** takes no command-line arguments, but can be configured using a
*crossloop.conf* file.

CONFIGURATION
=============
**crossloop** is configured using *crossloop.conf*, which is sourced verbatim
by a Bourne shell. The following configuration options are available:

*CF_BIN* (default "@bindir@/crossfire-server")
    Path to **crossfire-server** binary

*CF_FLAGS* (default "-d")
    Command-line arguments to pass to **crossfire-server**

*CF_LOG* (default "/tmp/crossfire/crossfire.log")
    Redirect standard output from **crossfire-server** to this log file

*CF_TMP* (default "/tmp/crossfire")
    Directory for **crossfire-server** temporary files. Will be created if it
    does not exist.

*CRASH_DIR* (default "/tmp")
    Make a copy of *CF_TMP* in this directory when a server crashes. Useful
    for preserving temporary files to investigate a crash.

*CRASH_MAIL* (unset by default)
    If set, send an email to the given address when **crossfire-server**
    crashes. If ``gdb`` is installed, the crash report includes a backtrace
    from the core dump and can be useful for debugging.

    A ``sendmail(8)``-compatible MTA must be installed locally for this to work.

*MAX_RESTART* (default 100)
    Exit **crossloop** after restarting **crossfire-server** this many times.
    This prevents a loop of crashes from using too many resources on the
    server.

EXAMPLE
=======
This example *crossloop.conf* enables crash mail to the specified email
address, and redirects the **crossfire-server** log to */var/log*::

    CF_LOG="/var/log/crossfire"
    CRASH_MAIL="nobody@example.com"

FILES
=====
@sysconfdir@/crossfire/crossloop.conf
    **crossloop** configuration file

SEE ALSO
========
crossfire-server(6) sendmail(8) gdb(1)
