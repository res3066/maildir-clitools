#!/usr/bin/bash

# usage: breakup_inbox.sh rs 2016 2022
#        breakup_inbox.sh user first_year final_year (to archive)


cd /home/$1/Maildir/cur

for year in $(seq $2 $3)
do
  date
  echo "archiving $year"
  doveadm mailbox create -u $1 Archives$year
  time ls | /home/$1/maildirfind.sh $year | /home/$1/bulkrename /home/$1/Maildir/.Archives$year/cur
  date
  echo "indexing $year (please be patient)"
  time doveadm index -u $1 Archives$year

done

