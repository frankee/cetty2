if (MSVC)
  # For MSVC, CMake sets certain flags to defaults we want to override.
  # This replacement code is taken from sample in the CMake Wiki at
  # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
  foreach (flag_var
           CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
           CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    # In hermetic build environments, tests may not have access to MS runtime
    # DLLs, so this replaces /MD (CRT libraries in DLLs) with /MT (static CRT
    # libraries).
    #string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")
    # We prefer more strict warning checking for building Google Test.
    # Replaces /W3 with /W4 in defaults.
    string(REPLACE "/W3" "-W4" ${flag_var} "${${flag_var}}")
  endforeach()
endif()

# Defines the compiler/linker flags used to build cetty.  You can
# tweak these definitions to suit your need.  A variable's value is
# empty before it's explicitly assigned to.

if (MSVC)
  # Newlines inside flags variables break CMake's NMake generator.
  #set(cxx_base_flags "-GS -W4 -WX -wd4275 -nologo -J -Zi")
  set(cxx_base_flags "-GS -W3 -wd4275 -wd4996 -nologo -Zi")
  set(cxx_base_flags "${cxx_base_flags} -D_UNICODE -DUNICODE -DWIN32 -D_WIN32 -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x0501")
  #set(cxx_base_flags "${cxx_base_flags} -DSTRICT")
  set(cxx_base_flags "${cxx_base_flags} -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -Dinline=__inline")
  set(cxx_exception_flags "-EHsc -D_HAS_EXCEPTIONS=1")
  set(cxx_no_exception_flags "-D_HAS_EXCEPTIONS=0")
  set(cxx_no_rtti_flags "-GR-")
elseif (CMAKE_COMPILER_IS_GNUCXX)
  set(cxx_base_flags "-Wall -Wno-shadow")
  set(cxx_exception_flags "-fexceptions")
  set(cxx_no_exception_flags "-fno-exceptions")
  # Until version 4.3.2, GCC doesn't define a macro to indicate
  # whether RTTI is enabled.  Therefore we define CETTY _HAS_RTTI
  # explicitly.
  set(cxx_no_rtti_flags "-fno-rtti -DCETTY_HAS_RTTI=0")
  set(cxx_strict_flags "-Wextra")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "SunPro")
  set(cxx_exception_flags "-features=except")
  # Sun Pro doesn't provide macros to indicate whether exceptions and
  # RTTI are enabled, so we define CETTY_HAS_* explicitly.
  set(cxx_no_exception_flags "-features=no%except -DCETTY_HAS_EXCEPTIONS=0")
  set(cxx_no_rtti_flags "-features=no%rtti -DCETTY_HAS_RTTI=0")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "VisualAge" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "XL")
  # CMake 2.8 changes Visual Age's compiler ID to "XL".
  set(cxx_exception_flags "-qeh")
  set(cxx_no_exception_flags "-qnoeh")
  # Until version 9.0, Visual Age doesn't define a macro to indicate
  # whether RTTI is enabled.  Therefore we define CETTY_HAS_RTTI
  # explicitly.
  set(cxx_no_rtti_flags "-qnortti -DCETTY_HAS_RTTI=0")
endif()

if (CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
  set(cxx_base_flags "${cxx_base_flags} -DCETTY_HAS_PTHREAD=1")
endif()

# For building cetty's own tests and samples.
set(cxx_exception "${CMAKE_CXX_FLAGS} ${cxx_base_flags} ${cxx_exception_flags}")
set(cxx_no_exception
    "${CMAKE_CXX_FLAGS} ${cxx_base_flags} ${cxx_no_exception_flags}")
set(cxx_default "${cxx_exception}")
set(cxx_no_rtti "${cxx_default} ${cxx_no_rtti_flags}")

# For building the cetty libraries.
set(cxx_strict "${cxx_default} ${cxx_strict_flags}")


if (CMAKE_COMPILER_IS_GNUCXX)
  set(CXX_FLAGS
    -g
    # -DVALGRIND
    -D_FILE_OFFSET_BITS=64
    -Wall
    #-Wextra
    #-Werror
    -Wconversion
    -Wno-unused-parameter
    #-Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    #-Wshadow
    -Wwrite-strings
    #-march=native
    # -MMD
    # -std=c++0x
    -rdynamic
    )
  if(CMAKE_BUILD_BITS EQUAL 32)
    list(APPEND CXX_FLAGS "-m32")
  endif()
  string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

  set(CMAKE_CXX_FLAGS_DEBUG "-O0")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
endif()
