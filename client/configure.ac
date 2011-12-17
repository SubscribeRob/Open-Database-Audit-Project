#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([Open Database Audit Project], [0.9.0], [www.opendbaudit.com])
AC_CONFIG_SRCDIR([src/CachedUserInfo.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

#Ensure boost is available
BOOST_REQUIRE([1.48.0])
BOOST_THREADS([s])
BOOST_REGEX([s])

#Check for log4cxx headers

AC_LOG4CXX_CHECK
AC_THRIFT_CHECK

#AC_CHECK_HEADERS([log4cxx/logger.h log4cxx/basicconfigurator.h log4cxx/helpers/exception.h],
#                 [break],
#                 [AC_MSG_ERROR([log4cxx headers not found. Ensure log4cxx is installed])])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-llog4cxx':
AC_CHECK_LIB([log4cxx], [main])
# FIXME: Replace `main' with a function in `-lpcap':
AC_CHECK_LIB([pcap], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SSIZE_T
AC_TYPE_UID_T

# Checks for library functions.
AC_CONFIG_MACRO_DIR([m4])


AC_CHECK_FUNCS([inet_ntoa strcasecmp])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

