#!/bin/sh
echo "insert root mysql password"
mysql -u root -p < storm_mysql_tbl.sql

host=`hostname -s` 
domain=`hostname`
tmp=`mktemp /tmp/sql.XXXXXX`

sed s/__HOST__/$host/g storm_mysql_grant.sql | \
sed s/__HOSTDOMAIN__/$domain/g > $tmp

mysql -u root -p < $tmp
rm -f $tmp

#
