#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([libdpl], [1.0], [schweizer@cmu.edu])
AC_CONFIG_SRCDIR([src/dpl.h])
AM_INIT_AUTOMAKE([-Wall -Werror])
AM_CONFIG_HEADER([config.h])
AC_DISABLE_STATIC

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL

# Checks for MPI
AC_MSG_CHECKING([for mpi.h])
AC_TRY_COMPILE([#include <mpi.h>],[],[AC_MSG_RESULT(yes)], [MPIH="no"
	AC_MSG_RESULT(no)])
if test "x$MPIH" = "xno" ; then
	AC_MSG_ERROR([MPI header files not found!])
else
	AC_DEFINE(HAVE_MPI_H,1,[Defined if you have mpi.h.])
fi

AC_CHECK_PROGS(MPICC, mpicc, [no])
if test "x$MPICC" = "xno" ; then
	AC_MSG_ERROR([No MPI compiler found!])
else
	CC="$MPICC"
	AC_DEFINE(HAVE_MPI,1,[Defined if you have the MPI library.])
fi

# Checks for libraries.
AC_ARG_WITH([openmp],
	AC_HELP_STRING([--with-openmp],[use openmp for local threads instead of MPI]),
	[ac_cv_use_openmp=$withval], [ac_cv_use_openmp=no])
if test "x$ac_cv_use_openmp" = "xno" ; then
	ac_cv_use_openmp=no
else
	CFLAGS="$CFLAGS -fopenmp"
	AC_MSG_CHECKING([for openmp])
	AC_TRY_COMPILE([#include <omp.h>],[],[AC_MSG_RESULT(yes)], [ac_cv_use_openmp="no"
		AC_MSG_RESULT(no)])
	if test "x$ac_cv_use_openmp" = "xno" ; then
		AC_MSG_ERROR([Configured with openmp but compiler does not seem to support it!])
	else
		AC_DEFINE(HAVE_OPENMP,1,[Use openmp for local threads.])
	fi
fi


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

if test "x$ac_cv_use_openmp" != "xno" ; then
	echo ""
	echo "*****************************************************************"
	echo "* You have  selected  to use openmp  for local  threads instead *"
	echo "* of MPI.  You should make sure that when  running your program *"
	echo "* that you do not spawn multiple processes on the same machine. *"
	echo "* This will  conflict with  openmp and  drastically  reduce the *"
	echo "* computing  power of your program.  You can  fix this by using *"
	echo "* the --bynode command line argument with openmpi or by writing *"
	echo "* machinefiles for mpich.                                       *"
	echo "*****************************************************************"
	echo ""
fi

