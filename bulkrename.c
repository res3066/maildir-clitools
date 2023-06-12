/*
   Meant as a companion for Vixie's maildirfind.sh but may have other uses.
   Ingests a bunch of filenames (assumed to be in current directory, no path names!)
   one per line on STDIN and moves them somewhere else (path to directory).
  
   Why not xargs and keep it all shell?  Forking is slow, asynchrous zfs
   is fast. We're hitting 1000 file renames a second on humble hardware (rotating rust,
   single vdev raidz2, no slog).
  
   MIT copyright 2023 by Rob Seastrom
  
   Usage:
   bulkrename [-vn] destdir_fullpath
   -v is verbose, -n is don't-actually-move-anything (dry run).

   Using with Dovecot cheat sheet:
   [root@staging /home/rs/Maildir/cur]#  doveadm mailbox create -u rs Archives2016
   [root@staging /home/rs/Maildir/cur]#  doveadm mailbox status -u rs all Archives2016
   [root@staging /home/rs/Maildir/cur]#  ls | maildirfind.sh 2016 | bulkrename [-vn] /home/rs/Maildir/.Archives2016/cur
   [root@staging /home/rs/Maildir/cur]#  doveadm -Dv index -u rs Archives2016
  
*/

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int main (int argc, char **argv) {
    int ret;
    int verbose = 0;
    int doit = 1;
    char destdir[PATH_MAX];
    char cwd[PATH_MAX];
    char fulldestpath[PATH_MAX];
    char fullsourcepath[PATH_MAX];
    char *theline;
    char c;
    DIR* dir;

    while ((c = getopt (argc, argv, "vn")) != -1)
	switch (c) {
	case 'v':
	    verbose = 1;
	    break;
	case 'n':
	    doit = 0;
	    break;
	case '?':
	    if (isprint (optopt))
		fprintf (stderr, "Unknown option '%c'.\n", optopt);
	    else
		fprintf (stderr,
			 "Unknown option character '%x'.\n",
			 optopt);
	    exit(1);
	default:
	    fprintf (stderr, "i got nuthin, aborting...\n");
	    exit(1);
	}
    
    if (optind != argc -1) {
	fprintf(stderr, "expecting exactly one non-flag argument -- the destination directory.\n");
	for ( ; optind < argc ; optind++) {
	    fprintf(stderr, "extra arguments: %s\n", argv[optind]);
	} 
	exit(1);
    } else {
        strncpy(destdir, argv[optind], sizeof(destdir));
    }
      
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
	perror("getcwd() error");
	exit(1);
    }
    
    if (access(destdir, W_OK) != 0) {
	fprintf(stderr, "Can't write to %s or it doesn't exist\n", destdir); 
        exit(1);
    }
    
    dir = opendir(destdir);
    if (!dir) {
	fprintf(stderr, "Couldn't open %s as a directory\n", destdir);
	exit(1);
    }
    closedir(dir);

    if (verbose) {
	fprintf(stderr, "Current directory: %s\n", cwd);
	fprintf(stderr, "Destination directory: %s\n", destdir);
	fprintf(stderr, "Let's move some files!\n");
    } 

/* set up, command line parsed, etc.
   now we read stdin and loop until eof
   and try to do a rename with each line we read.
 */

    theline=malloc(PATH_MAX);

    while (fgets(theline, PATH_MAX, stdin) != NULL) {

	theline[strcspn(theline, "\n")] = 0; // stomp on trailing \n

/* sloppy, but close enough for government work -
   no buffer overflows as we put together the full paths
*/

	if (strlen(destdir) + strlen("/") + strlen(theline) >= PATH_MAX - 1) {
	    fprintf(stderr, "Bailing out  because PATH_MAX won't accommodate destination file %s/%s\n", destdir, theline);
	    exit(1);
	}
	
	if (strlen(cwd) + strlen("/") + strlen(theline) >= PATH_MAX - 1) {
	    fprintf(stderr, "Bailing out  because PATH_MAX won't accommodate source file %s/%s\n", cwd, theline);
	    exit(1);
	}
	
	strncpy(fulldestpath, destdir, sizeof(fulldestpath));
	if (destdir[strlen(destdir)-1] != '/')
	{
	    strncat(fulldestpath, "/", sizeof(fulldestpath) - 1);
	}
	strncat(fulldestpath, theline, sizeof(fulldestpath) - 1);

/* that trailing slash won't happen in practice but can't be too careful */
	strncpy(fullsourcepath, cwd, sizeof(fullsourcepath));
	if (cwd[strlen(cwd)-1] != '/')
	{
	    strncat(fullsourcepath, "/", sizeof(fullsourcepath) - 1);
	}
	strncat(fullsourcepath, theline, sizeof(fullsourcepath) - 1);


	if (verbose) {
	    fprintf(stderr, "Moving %s to %s\n", fullsourcepath, fulldestpath);
	}

	if (doit) {

	    ret = rename(fullsourcepath, fulldestpath);

	    if(ret == 0) {
		if (verbose) {
		    printf("File renamed successfully\n");
		}
	    } else {
		fprintf(stderr, "Error: unable to rename %s to %s, giving up\n", fullsourcepath, fulldestpath);
		exit(1);
	    }
	}
    }
    exit(0);
}

