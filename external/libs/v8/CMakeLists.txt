cmake_minimum_required(VERSION 3.6)

set(lib_name v8)
set(target_name ext_${lib_name})

project(${lib_name})

include(../../cmake/CocosExternalConfig.cmake)

add_library(${target_name} STATIC IMPORTED GLOBAL)

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
  	CC_DEPEND_DLLS "${platform_spec_path}/v8.dll"
  	CC_DEPEND_DLLS "${platform_spec_path}/v8_libbase.dll"
  	CC_DEPEND_DLLS "${platform_spec_path}/v8_libplatform.dll"
    IMPORTED_LOCATION "${platform_spec_path}/v8.dll.lib"
    IMPORTED_LOCATION "${platform_spec_path}/v8_libbase.dll.lib"
    IMPORTED_LOCATION "${platform_spec_path}/v8_libplatform.dll.lib"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${platform_spec_path}/libinspector.a"
    IMPORTED_LOCATION "${platform_spec_path}/libv8_base.a"
    IMPORTED_LOCATION "${platform_spec_path}/libv8_builtins_generators.a"
    IMPORTED_LOCATION "${platform_spec_path}/libv8_builtins_setup.a"
    IMPORTED_LOCATION "${platform_spec_path}/libv8_libbase.a"
    IMPORTED_LOCATION "${platform_spec_path}/libv8_libplatform.a"
    IMPORTED_LOCATION "${platform_spec_path}/libv8_libsampler.a"
    IMPORTED_LOCATION "${platform_spec_path}/libv8_nosnapshot.a"
  )
endif()
