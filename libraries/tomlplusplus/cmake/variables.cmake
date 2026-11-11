
set(tomlplusplus_warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(
      tomlplusplus_INCLUDES_WITH_SYSTEM
      "Use SYSTEM modifier for tomlplusplus's includes, disabling warnings"
      ON
  )
  mark_as_advanced(tomlplusplus_INCLUDES_WITH_SYSTEM)
  if(tomlplusplus_INCLUDES_WITH_SYSTEM)
    set(tomlplusplus_warning_guard SYSTEM)
  endif()
endif()
