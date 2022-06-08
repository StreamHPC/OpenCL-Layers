add_executable (CLInputVerificationGenerator
  parser.cpp
)
target_link_libraries (CLInputVerificationGenerator PRIVATE RapidXml)

add_custom_command (
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
  COMMAND
    CLInputVerificationGenerator ${CMAKE_CURRENT_SOURCE_DIR}/cl-avl.xml
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/cl-avl.xml
)

add_library (
  CLParamVerificationLayer
  SHARED
  ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
)
