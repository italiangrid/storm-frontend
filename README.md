The StoRM Frontend Service
===============================

StoRM Frontend provides an implementation of the SRM v2.2 Web Service interface.

## Supported platforms

* Scientific Linux 5 x86_64
* Scientific Linux 6 x86_64

## Required repositories

In order to build the frontend, please enable the following repositories on your build machine

### EPEL

```bash
yum install epel-release
```

### EMI 3

Get the latest production emi-release package from the EMI repository website:

http://emisoft.web.cern.ch/emisoft/index.html

```bash
yum localinstall $emi_release_package
```

#### Configure the latest EMI 3 testing repo

Repo files:

* SL6 http://eticssoft.web.cern.ch/eticssoft/mock/emi-3-rc-sl6.repo
* SL5 http://eticssoft.web.cern.ch/eticssoft/mock/emi-3-rc-sl5.repo 


```bash
wget -q $repo_file -o /etc/yum.repos.d/emi-testing.repo
yum clean all
yum update
yum install emi-release
```


### Build instructions

#### Build-time dependencies

Install the following required packages:

* git
* gcc
* gcc-c++
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

SL6 only:

* libuuid-devel
* boost-devel

SL5 only:

* boost141-devel

#### Build commands

##### SL5 

```bash
LDFLAGS=-L/usr/lib64/boost141 ./configure --with-boost=/usr/include/boost141 
make
```

#### SL6

```bash
./configure
make
```
Use the `--enable-debug` configure flag to include debugging symbols and turn
off optimization during development.

# Contact info

If you have problems, questions, ideas or suggestions, please contact us at
the following URLs

* GGUS (official support channel): http://www.ggus.eu
