find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

flex_target(ng_css_l ng_css.l ${CMAKE_CURRENT_BINARY_DIR}/ng_css_l.c
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/ng_css_l.h)
bison_target(ng_css_y ng_css.y ${CMAKE_CURRENT_BINARY_DIR}/ng_css_y.c)
add_flex_bison_dependency(ng_css_l ng_css_y)

add_library(ng_css ${BISON_ng_css_y_OUTPUTS} ${FLEX_ng_css_l_OUTPUTS}
  ng_str.h
  ng_str.c)
target_include_directories(ng_css
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
