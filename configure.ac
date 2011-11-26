# gpshub autoconf configure.ac

AC_PREREQ([2.67])
AC_INIT([gpshub], [0.7.1_beta], [misieks@gmail.com])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
#AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_create], [], [
              echo "pthread library missing!"
              exit -1])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday localtime_r memset socket strerror])

# this makes possible to enable debug compilation
AC_ARG_ENABLE(debug, 
              AS_HELP_STRING([--enable-debug], 
                             [enable debugging, default: no]), 
              [case "${enableval}" in 
               yes) debug=true ;;
               no) debug=false ;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
              esac],
              [debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile include/Makefile src/Makefile])
AC_OUTPUT


