#!/bin/sh -e

aclocal
autoheader
autoconf
automake -a

