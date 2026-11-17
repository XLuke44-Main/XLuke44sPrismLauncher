
# under the folowing license:

# MIT License

# Copyright (c) 2022-2100 Amin Yahyaabadi

# Permission is hereby granted, free of charge, to any person obtaining a copy

# The above copyright notice and this permission notice shall be included in all

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

include_guard()

function(_set_project_warnings_add_target_link_option TARGET OPTIONS)
  target_link_options(${_project_name} INTERFACE ${OPTIONS})
endfunction()

function(
  set_project_warnings
  _project_name
  MSVC_WARNINGS
  CLANG_WARNINGS
  GCC_WARNINGS
)
  if("${MSVC_WARNINGS}" STREQUAL "")
    set(MSVC_WARNINGS
        /W4  
        /w14242  
        /w14254  
        /w14263  
        /w14265  

        /w14287  
        /we4289  

        /w14296  
        /w14311  
        /w14545  
        /w14546  
        /w14547  
        /w14549  
        /w14555  
        /w14619  
        /w14640  
        /w14826  
        /w14905  
        /w14906  
        /w14928  
        /permissive-  

        /we4062  
    )
  endif()

  if("${CLANG_WARNINGS}" STREQUAL "")
    set(CLANG_WARNINGS
        -Wall
        -Wextra  
        -Wshadow  
        -Wnon-virtual-dtor  

        -Wold-style-cast  
        -Wcast-align  
        -Wunused  
        -Woverloaded-virtual  
        -Wpedantic  
        -Wconversion  
        -Wsign-conversion  
        -Wnull-dereference  
        -Wdouble-promotion  
        -Wformat=2  
        -Wimplicit-fallthrough  

        -Wno-gnu-zero-variadic-macro-arguments

        -Werror=switch  
    )
  endif()

  if("${GCC_WARNINGS}" STREQUAL "")
    set(GCC_WARNINGS
        ${CLANG_WARNINGS}
        -Wmisleading-indentation  
        -Wduplicated-cond  
        -Wduplicated-branches  
        -Wlogical-op  
        -Wuseless-cast  

        -Werror=switch  
    )
  endif()

  if(MSVC)
    set(PROJECT_WARNINGS_CXX ${MSVC_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(PROJECT_WARNINGS_CXX ${CLANG_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_WARNINGS_CXX ${GCC_WARNINGS})
  else()
    message(AUTHOR_WARNING "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")

  endif()

  set(PROJECT_WARNINGS_C "${PROJECT_WARNINGS_CXX}")
  list(
    REMOVE_ITEM
    PROJECT_WARNINGS_C
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wuseless-cast
    -Wextra-semi

    -Werror=switch  
  )

  target_compile_options(
    ${_project_name}
    INTERFACE  
              $<$<COMPILE_LANGUAGE:CXX>:${PROJECT_WARNINGS_CXX}>

              $<$<COMPILE_LANGUAGE:C>:${PROJECT_WARNINGS_C}>
   )

  if(CMAKE_CXX_LINK_EXECUTABLE MATCHES "^<CMAKE_CXX_COMPILER>")
    _set_project_warnings_add_target_link_option(
      ${_project_name} "$<$<COMPILE_LANGUAGE:CXX>:${PROJECT_WARNINGS_CXX}>"
    )
  endif()

  if(CMAKE_C_LINK_EXECUTABLE MATCHES "^<CMAKE_C_COMPILER>")
    _set_project_warnings_add_target_link_option(
      ${_project_name} "$<$<COMPILE_LANGUAGE:C>:${PROJECT_WARNINGS_C}>"
    )
  endif()

 endfunction()
