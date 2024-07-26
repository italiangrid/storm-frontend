#!/usr/bin/env bash

# Copyright 2023 Istituto Nazionale di Fisica Nucleare
# SPDX-License-Identifier: EUPL-1.2

set -ex

# Add repo for devtools and git 2.x
yum install -y \
    centos-release-scl \
    epel-release \
    https://packages.endpointdev.com/rhel/7/os/${ARCH}/endpoint-repo.${ARCH}.rpm \
    http://repository.egi.eu/sw/production/umd/4/centos7/x86_64/updates/umd-release-4.1.3-1.el7.centos.noarch.rpm

yum update -y

yum install -y --setopt=tsflags=nodocs \
    sudo \
    git \
    wget \
    curl \
    zip \
    unzip \
    tar \
    rpm-build \
    devtoolset-"${DEVTOOLSET_VERSION}" \
    devtoolset-"${DEVTOOLSET_VERSION}"-libubsan-devel \
    devtoolset-"${DEVTOOLSET_VERSION}"-libasan-devel \
    autoconf \
    automake \
    make \
    libtool \
    openssl-devel \
    gsoap-devel \
    curl-devel \
    mysql-devel \
    globus-gssapi-gsi-devel \
    globus-gss-assist-devel \
    globus-common-devel \
    globus-gridmap-callout-error-devel \
    globus-gsi-credential-devel \
    CGSI-gSOAP-devel \
    storm-xmlrpc-c-devel \
    argus-pep-api-c \
    argus-pep-api-c-devel \
    libuuid-devel \
    boost-devel

yum clean all
rm -rf /var/cache/yum