maildir-clitools
================

By the time you discover that a 750k email message inbox behind
your imap server is a bad plan, you're past the point of being
able to fix it over imap protocol and the imap server.

Timeouts because you tried
to move 100k messages at a clip, bumping your head on memory limits,
etc. will burn you.  But you're a Unix whiz from way back and aren't
afraid to take the bull by the horns and fix it from the command line
behind the imap server's back.

Dependencies
------------

Both shell scripts expect bash for a shell.

maildirfind.sh (by Vixie) originally expected bsd date.  It now expects
gnu date (from gnu coreutils).  It also had the inner loop ported
to gnu awk (gawk) for a 2.5 order of magnitude speedup.

bulkrename.c - plain c, standard library...

All of this has been tested on SmartOS (OpenSolaris derivative) with Dovecot
as the imap server and is opinionated about it.

Usage
----

Add the directory with the checked out Git project to your $PATH.

gcc -Wall -o bulkrename bulkrename.c

Read and understand breakup_inbox.sh which should document nicely how
the other parts fit together.

Test in staging.  Have good backups and snapshots. You've been warned.

License
-------

MIT

Author Information
------------------

Original concept by Paul Vixie
Expanded by Rob Seastrom
To atone for the sins of certain unnamed ClueTrust customers


