#!/usr/bin/bash
#
# first set your path to include the git directory...
#
# [root@staging /home/rs/maildir-clitools]# PATH=`pwd`:$PATH
#
# usage: breakup_inbox.sh rs 2016 2022
#        breakup_inbox.sh user first_year final_year (to archive)


cd /home/$1/Maildir/cur

for year in $(seq $2 $3)
do
  date
  echo "archiving $year"
  doveadm mailbox create -u $1 Archives$year
  time ls | maildirfind.sh $year | bulkrename /home/$1/Maildir/.Archives$year/cur
  date
  echo "indexing $year (please be patient)"
  time doveadm index -u $1 Archives$year

done

date
echo "rebuilding index for INBOX (please be patient one final time)"
time doveadm index -u $1 INBOX

exit 0

