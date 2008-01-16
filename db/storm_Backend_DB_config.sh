#!/bin/sh
mysql -u root -p < storm_mysql_tbl.sql
mysql -u root -p < storm_mysql_grant.sql
#
