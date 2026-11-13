
# The MIT License (MIT)

# Copyright (c) 2013 Matthew Arsenault

# Permission is hereby granted, free of charge, to any person obtaining a copy

# The above copyright notice and this permission notice shall be included in

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

include(CheckCCompilerFlag)

set(CMAKE_REQUIRED_FLAGS "-Werror -faddress-sanitizer")  
check_c_compiler_flag("-faddress-sanitizer" HAVE_FLAG_ADDRESS_SANITIZER)

set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=address")  
check_c_compiler_flag("-fsanitize=address" HAVE_FLAG_SANITIZE_ADDRESS)

unset(CMAKE_REQUIRED_FLAGS)

if(HAVE_FLAG_SANITIZE_ADDRESS)

  set(ADDRESS_SANITIZER_FLAG "-fsanitize=address")
elseif(HAVE_FLAG_ADDRESS_SANITIZER)

  set(ADDRESS_SANITIZER_FLAG "-faddress-sanitizer")
endif()

if(NOT ADDRESS_SANITIZER_FLAG)
  return()
else(NOT ADDRESS_SANITIZER_FLAG)
  set(HAVE_ADDRESS_SANITIZER FALSE)
endif()

set(HAVE_ADDRESS_SANITIZER TRUE)

set(CMAKE_C_FLAGS_ASAN "-O1 -g ${ADDRESS_SANITIZER_FLAG} -fno-omit-frame-pointer -fno-optimize-sibling-calls"
    CACHE STRING "Flags used by the C compiler during ASan builds."
    FORCE)
set(CMAKE_CXX_FLAGS_ASAN "-O1 -g ${ADDRESS_SANITIZER_FLAG} -fno-omit-frame-pointer -fno-optimize-sibling-calls"
    CACHE STRING "Flags used by the C++ compiler during ASan builds."
    FORCE)
set(CMAKE_EXE_LINKER_FLAGS_ASAN "${ADDRESS_SANITIZER_FLAG}"
    CACHE STRING "Flags used for linking binaries during ASan builds."
    FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_ASAN "${ADDRESS_SANITIZER_FLAG}"
    CACHE STRING "Flags used by the shared libraries linker during ASan builds."
    FORCE)
mark_as_advanced(CMAKE_C_FLAGS_ASAN
                 CMAKE_CXX_FLAGS_ASAN
                 CMAKE_EXE_LINKER_FLAGS_ASAN
                 CMAKE_SHARED_LINKER_FLAGS_ASAN)
