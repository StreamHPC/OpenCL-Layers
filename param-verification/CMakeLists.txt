add_executable (CLParamVerificationGenerator
  parser.cpp
)
target_link_libraries (CLParamVerificationGenerator PRIVATE RapidXml)

add_custom_command (
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
  COMMAND
    CLParamVerificationGenerator ${CMAKE_CURRENT_SOURCE_DIR}/cl-avl.xml
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/cl-avl.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp
)

add_library (
  CLParamVerificationLayer
  SHARED
  ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
)
target_include_directories(CLParamVerificationLayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
