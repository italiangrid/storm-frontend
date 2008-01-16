#!/bin/sh
# $Id$
#*********************************************************************
#*
#* Authors: Flavia Donno <Flavia.Donno@cern.ch>
#*
#* Copyright 2004 INFN FIRB Project
#*           http://grid-it.cnaf.infn.it/
#*
#* Date: 01 March 2005
#*
#*********************************************************************
set -x
if [ ! -x ./config ] ; then
   mkdir ./config
fi

rm -f config.cache

aclocal -I config 2>/dev/null
autoheader
libtoolize -c --automake
automake --foreign --add-missing --copy 2>/dev/null
autoconf
#
