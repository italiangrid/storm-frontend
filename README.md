The StoRM Frontend Service
==========================

StoRM Frontend provides an implementation of the SRM v2.2 Web Service interface.

## Supported platforms

* CentOS 6 x86_64
* CentOS 7 x86_64

## Required repositories

In order to build the frontend, please enable the following repositories on your build machine

### EPEL

    yum install epel-release

### UMD

Get the latest production packages from the UMD repository website:

http://repository.egi.eu/category/umd_releases/distribution/umd-4/

## Build-time dependencies

Install the following required packages:

* git
* gcc
* gcc-c++
* autoconf
* automake
* make
* gsoap-devel
* curl-devel
* mysql-devel
* globus-gssapi-gsi-devel
* globus-gss-assist-devel
* globus-common-devel
* globus-gridmap-callout-error-devel
* globus-gsi-credential-devel
* CGSI-gSOAP-devel
* storm-xmlrpc-c-devel
* argus-pep-api-c
* argus-pep-api-c-devel
* libuuid-devel
* boost-devel

## Build commands

    ./bootstrap
    ./configure
    make

Configure with `--with-debug` to compile in debug mode.

## Contact info

If you have problems, questions, ideas or suggestions, please contact us at
the following URL

* GGUS (official support channel): https://www.ggus.eu
