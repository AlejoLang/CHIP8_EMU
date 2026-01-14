add_executable(${PROJECT_NAME}
  main.cpp
  Display.cpp
  Memory.cpp
  Keypad.cpp
  CHIP8.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)