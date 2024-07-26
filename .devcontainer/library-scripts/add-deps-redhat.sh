#!/usr/bin/env bash
#
# Copyright (c) Istituto Nazionale di Fisica Nucleare
# Licensed under the EUPL
#
# Syntax: ./add-deps-redhat.sh

set -e

package_list="\
  autoconf \
  autoconf-archive \
  automake \
  bear \
  bison \
  boost-devel \
  diffutils \
  CGSI-gSOAP-devel \
  file \
  gcc-c++ \
  gdb \
  git \
  globus-common-devel \
  gsoap-devel \
  libtool \
  libuuid-devel \
  make \
  mariadb-devel \
  openssl-devel \
  xmlrpc-c-devel"

dnf install -y ${package_list}
