#/****************************************************************************
# Copyright (c) 2014-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

set(cocos_root ${COCOS2DX_ROOT_PATH})

#platform
if(APPLE)# osx or ios
    set(PLATFORM_HEADER manual/JavaScriptObjCBridge.h)
    set(PLATFORM_SRC manual/JavaScriptObjCBridge.mm)
elseif(ANDROID)
    set(PLATFORM_HEADER manual/JavaScriptJavaBridge.h)
    set(PLATFORM_SRC manual/JavaScriptJavaBridge.cpp)
    # refer to: cocos/scripting/js-bindings/proj.android
endif()

if(IOS)
    set(PLATFORM_HEADER
        ${PLATFORM_HEADER}
        auto/jsb_cocos2dx_experimental_webView_auto.hpp
        auto/jsb_cocos2dx_experimental_video_auto.hpp
        manual/jsb_platform_ios.h
        )
    set(PLATFORM_SRC
        ${PLATFORM_SRC}
        auto/jsb_cocos2dx_experimental_webView_auto.cpp
        auto/jsb_cocos2dx_experimental_video_auto.cpp
        manual/jsb_platform_ios.m
        )
endif()

if(ANDROID OR WINDOWS)
    set(JSBINDING_PLATFORM_SPECIFIC_HEADER
        jswrapper/v8/Base.h
        jswrapper/v8/base64.h
        jswrapper/v8/Class.hpp
        jswrapper/v8/env.h
        jswrapper/v8/HelperMacros.h
        jswrapper/v8/http_parser.h
        jswrapper/v8/inspector_agent.h
        jswrapper/v8/inspector_io.h
        jswrapper/v8/inspector_socket_server.h
        jswrapper/v8/inspector_socket.h
        jswrapper/v8/node_debug_options.h
        jswrapper/v8/node_mutex.h
        jswrapper/v8/node.h
        jswrapper/v8/Object.hpp
        jswrapper/v8/ObjectWrap.h
        jswrapper/v8/ScriptEngine.hpp
        jswrapper/v8/SeApi.h
        jswrapper/v8/SHA1.h
        jswrapper/v8/util-inl.h
        jswrapper/v8/util.h
        jswrapper/v8/Utils.hpp
        jswrapper/v8/v8_inspector_protocol_json.h
    )
    set(JSBINDING_PLATFORM_SPECIFIC_SRC
        jswrapper/v8/Class.cpp
        jswrapper/v8/env.cc
        jswrapper/v8/http_parser.c
        jswrapper/v8/inspector_agent.cc
        jswrapper/v8/inspector_io.cc
        jswrapper/v8/inspector_socket_server.cc
        jswrapper/v8/inspector_socket.cc
        jswrapper/v8/node_debug_options.cc
        jswrapper/v8/node.cc
        jswrapper/v8/Object.cpp
        jswrapper/v8/ObjectWrap.cpp
        jswrapper/v8/ScriptEngine.cpp
        jswrapper/v8/SHA1.cpp
        jswrapper/v8/util.cc
        jswrapper/v8/Utils.cpp
    )
elseif(IOS)
    set(JSBINDING_PLATFORM_SPECIFIC_HEADER
        jswrapper/jsc/Base.h
        jswrapper/jsc/Class.hpp
        jswrapper/jsc/HelperMacros.h
        jswrapper/jsc/Object.hpp
        jswrapper/jsc/PlatformUtils.h
        jswrapper/jsc/ScriptEngine.hpp
        jswrapper/jsc/SeApi.h
        jswrapper/jsc/Utils.hpp
    )
    set(JSBINDING_PLATFORM_SPECIFIC_SRC
        jswrapper/jsc/Class.cpp
        jswrapper/jsc/Object.cpp
        jswrapper/jsc/PlatformUtils.mm
        jswrapper/jsc/ScriptEngine.cpp
        jswrapper/jsc/Utils.cpp
    )
endif()

set(JSBINDING_WRAPPER_HEADER
    jswrapper/config.hpp
    jswrapper/HandleObject.hpp
    jswrapper/MappingUtils.hpp
    jswrapper/Object.hpp
    jswrapper/RefCounter.hpp
    jswrapper/SeApi.h
    jswrapper/State.hpp
    jswrapper/Value.hpp
    )
set(JSBINDING_WRAPPER_SRC
    jswrapper/config.cpp
    jswrapper/HandleObject.cpp
    jswrapper/MappingUtils.cpp
    jswrapper/RefCounter.cpp
    jswrapper/State.cpp
    jswrapper/Value.cpp
    )

set(JSBINDING_HEADER
    ${PLATFORM_HEADER}
    ${JSBINDING_PLATFORM_SPECIFIC_HEADER}
    ${JSBINDING_WRAPPER_HEADER}
    auto/jsb_cocos2dx_auto.hpp
    #auto/jsb_cocos2dx_dragonbones_auto.hpp
    auto/jsb_cocos2dx_extension_auto.hpp
    auto/jsb_cocos2dx_network_auto.hpp
    auto/jsb_cocos2dx_spine_auto.hpp
    auto/jsb_cocos2dx_ui_auto.hpp
    auto/jsb_creator_auto.hpp
    auto/jsb_creator_camera_auto.hpp
    auto/jsb_creator_graphics_auto.hpp
    #auto/jsb_physics_auto.hpp
    manual/BaseJSAction.h
    #manual/jsb_box2d_manual.hpp
    manual/jsb_classtype.hpp
    manual/jsb_cocos2dx_audioengine_manual.hpp
    manual/jsb_cocos2dx_extension_manual.hpp
    manual/jsb_cocos2dx_manual.hpp
    manual/jsb_conversions.hpp
    manual/jsb_creator_manual.hpp
    #manual/jsb_creator_physics_manual.hpp
    #manual/jsb_dragonbones_manual.hpp
    manual/jsb_global.h
    manual/jsb_helper.hpp
    manual/jsb_module_register.hpp
    manual/jsb_node.hpp
    manual/jsb_opengl_functions.hpp
    manual/jsb_opengl_manual.hpp
    manual/jsb_opengl_node.hpp
    manual/jsb_opengl_registration.hpp
    manual/jsb_socketio.hpp
    manual/jsb_spine_manual.hpp
    manual/jsb_websocket.hpp
    manual/jsb_xmlhttprequest.hpp
    manual/ScriptingCore.h
    )

set(JSBINDING_SRC
    ${PLATFORM_SRC}
    ${JSBINDING_PLATFORM_SPECIFIC_SRC}
    ${JSBINDING_WRAPPER_SRC}
    auto/jsb_cocos2dx_auto.cpp
    #auto/jsb_cocos2dx_dragonbones_auto.cpp
    auto/jsb_cocos2dx_extension_auto.cpp
    auto/jsb_cocos2dx_network_auto.cpp
    auto/jsb_cocos2dx_spine_auto.cpp
    auto/jsb_creator_auto.cpp
    auto/jsb_creator_camera_auto.cpp
    auto/jsb_creator_graphics_auto.cpp
    #auto/jsb_physics_auto.cpp
    manual/BaseJSAction.cpp
    #manual/jsb_box2d_manual.cpp
    manual/jsb_classtype.cpp
    manual/jsb_cocos2dx_audioengine_manual.cpp
    manual/jsb_cocos2dx_extension_manual.cpp
    manual/jsb_cocos2dx_manual.cpp
    manual/jsb_conversions.cpp
    manual/jsb_creator_manual.cpp
    #manual/jsb_creator_physics_manual.cpp
    #manual/jsb_dragonbones_manual.cpp
    manual/jsb_global.cpp
    manual/jsb_helper.cpp
    manual/jsb_module_register.cpp
    manual/jsb_node.cpp
    manual/jsb_opengl_functions.cpp
    manual/jsb_opengl_manual.cpp
    manual/jsb_opengl_node.cpp
    manual/jsb_opengl_registration.cpp
    manual/jsb_socketio.cpp
    manual/jsb_spine_manual.cpp
    manual/jsb_websocket.cpp
    manual/jsb_xmlhttprequest.cpp
    manual/ScriptingCore.cpp
    )

list(APPEND JSBINDING_SRC ${JSBINDING_HEADER} ${PLATFORM_HEADER})

add_library(jscocos2d STATIC
            ${JSBINDING_SRC}
            )

target_link_libraries(jscocos2d cocos2d)
add_dependencies(jscocos2d cocos2d)

get_target_property(js_ext_depend external CC_JS_DEPEND)
target_link_libraries(jscocos2d ${js_ext_depend})

target_include_directories(jscocos2d
        INTERFACE ${cocos_root}/cocos/scripting/js-bindings/auto
        INTERFACE ${cocos_root}/cocos/scripting/js-bindings/manual
)

if(WINDOWS)
    target_compile_definitions(jscocos2d PRIVATE _USRJSSTATIC)
endif()

set_target_properties(jscocos2d
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      FOLDER "Internal"
                      )

if(XCODE OR VS)
    cocos_mark_code_files("jscocos2d")
endif()