# gpshub automake Makefile.am

# foreign is to let automake know that it is not a standard GNU package
AUTOMAKE_OPTIONS = foreign

SUBDIRS = include src

EXTRA_DIST = doc test

dist-hook:
		rm -rf `find $(distdir) -name .svn`

# Define an independent executable script for inclusion in the distribution
# archive. It will not be installed on an end user's system, however.
dist_noinst_SCRIPTS = autogen.sh

