# AC_COMPILER add switches to enable debug mode, profiling, warnings,
#             sanitizers, coverage, ...
# ----------------------------------------------------------------------------
AC_DEFUN([AC_COMPILER],
[
    # enable warning inconditionally
    CFLAGS="${CFLAGS} -Wall -Wextra"
    CXXFLAGS="${CXXFLAGS} -Wall -Wextra"

    AC_ARG_WITH(debug,
      [  --with-debug Compile without optimization and in debug mode],
      [ac_with_debug="yes"],
      [ac_with_debug="no"])
    
    if test "x$ac_with_debug" = "xyes" ; then
      CFLAGS="${CFLAGS} -g -O0"
      CXXFLAGS="${CXXFLAGS} -g -O0"
    fi

    AC_ARG_WITH(profile,
      [  --with-profile Enable collection of profiling information],
      [ac_with_profile="yes"],
      [ac_with_profile="no"])
    
    if test "x$ac_with_profile" = "xyes" ; then
      CFLAGS="${CFLAGS} -pg"
      CXXFLAGS="${CXXFLAGS} -pg"
      LDFLAGS="${LDFLAGS} -pg"
    fi

    AC_ARG_WITH(coverage,
      [  --with-coverage Enable collection of coverage information],
      [ac_with_coverage="yes"],
      [ac_with_coverage="no"])

    if test "x$ac_with_coverage" = "xyes" ; then
      CFLAGS="${CFLAGS} --coverage"
      CXXFLAGS="${CXXFLAGS} --coverage"
      LDFLAGS="${LDFLAGS} --coverage"
    fi
])
