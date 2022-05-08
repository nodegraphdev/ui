find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(NG_BISON_FLAGS "-tv")
  add_compile_definitions(YYDEBUG=1)
else()
  set(NG_BISON_FLAGS "")
endif()

flex_target(ng_css_l ng_css.l ${CMAKE_CURRENT_BINARY_DIR}/ng_css_l.c
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/ng_css_l.h)
bison_target(ng_css_y ng_css.y ${CMAKE_CURRENT_BINARY_DIR}/ng_css_y.c
  COMPILE_FLAGS ${NG_BISON_FLAGS})
add_flex_bison_dependency(ng_css_l ng_css_y)

add_library(ng_css ${BISON_ng_css_y_OUTPUTS} ${FLEX_ng_css_l_OUTPUTS}
  ng_str.h
  ng_str.c
  ng_properties.h
  ng_properties.c
  ng_css.c)
target_include_directories(ng_css
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
