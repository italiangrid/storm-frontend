#!/bin/sh
export STORM_DIR=/home/storm
export GLOBUS_LOCATION=/opt/globus
export GPT_LOCATION=/opt/gpt
export GLITE_LOCATION=/opt/glite
export GSOAPGSI_DIR=/usr
export GSOAP_DIR=/usr/local/gsoap-2.7/bin
export MYSQL_DIR=/usr
export MYSQLAPI_DIR=/opt/mysql++
export XMLRPC_C_DIR=$STORM_DIR/xmlrpc-c_new
export CURL_LOCATION=$STORM_DIR/curl_new
export PATH=$PATH:$XMLRPC_C_DIR/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GLITE_LOCATION/lib
