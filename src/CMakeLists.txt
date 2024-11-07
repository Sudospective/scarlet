list(APPEND ENGINE_MAIN
  main.cpp
)

add_executable(${PROJECT_NAME}
  ${ENGINE_MAIN}
)

if(WIN32)
  
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
      $<TARGET_FILE_DIR:${PROJECT_NAME}>
    USES_TERMINAL
    COMMAND_EXPAND_LISTS
  )
endif()
