--###################################################
--# Copyright (c) 2005 on behalf of the INFNGRID Project: 
--# The Italian National Institute for Nuclear Physics (INFN), 
--# All rights reserved.
--#
--# createrole sql script for a database
--#
--# author:    flavia.donno@cern.ch
--#            luca.magnoni@cnaf.infn.it
--# changelog: Added grant permission on storm_be_ISAM database.
--#
--#
--###################################################
USE mysql;
GRANT ALL PRIVILEGES ON storm_db.* TO storm IDENTIFIED BY 'storm' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON storm_db.* TO storm@'localhost' IDENTIFIED BY 'storm' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON storm_db.* TO storm@'__HOST__' IDENTIFIED BY 'storm' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON storm_db.* TO storm@'__HOSTDOMAIN__' IDENTIFIED BY 'storm' WITH GRANT OPTION;

GRANT ALL PRIVILEGES ON storm_be_ISAM.* TO storm IDENTIFIED BY 'storm' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON storm_be_ISAM.* TO storm@'localhost' IDENTIFIED BY 'storm' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON storm_be_ISAM.* TO storm@'__HOST__' IDENTIFIED BY 'storm' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON storm_be_ISAM.* TO storm@'__HOSTDOMAIN__' IDENTIFIED BY 'storm' WITH GRANT OPTION;


FLUSH PRIVILEGES;


