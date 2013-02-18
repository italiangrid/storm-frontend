The StoRM BackEnd Service
===============================

StoRM FrontEnd exposes an implementation of the SRM v2.2 interface via Web Services.
This frontend component interact with StoRM BackEnd to provide an SRM Grid Storage Element

Supported platforms
Scientific Linux 5 on x86_64 architecture
Scientific Linux 6 on x86_64 architecture

### Building
Required packages:

* epel
* git
* gcc
* gcc-c++
* automake
* autoconf
* libtool
* boost141-devel
* gsoap-devel
* curl-devel
* mysql-devel
* globus-gssapi-gsi-devel
* globus-gss-assist-devel
* globus-common-devel
* globus-gridmap-callout-error-devel
* globus-gsi-credential-devel
* CGSI-gSOAP-devel
* voms
* storm-xmlrpc-c-devel
* argus-pep-api-c
* argus-pep-api-c-devel
* rpm-build

SL6 only
* libuuid-devel

Build command:
* on SL55

```bash
./bootstrap
./configure --with-gsoap-version=2.7.13 --with-gsoap-wsdl2h-version=1.2.13
make rpm
```
* on SL56 

```bash
./bootstrap
./configure --with-gsoap-version=2.7.16 --with-gsoap-wsdl2h-version=1.2.16 --with-libuuid-devel=yes
make rpm
```

# Contact info

If you have problems, questions, ideas or suggestions, please contact us at
the following URLs

* GGUS (official support channel): http://www.ggus.eu
