cmake_minimum_required(VERSION 3.20)
project(NReflection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${MODE_PATH}/binarys")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${MODE_PATH}/binarys")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${MODE_PATH}/archive")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/${MODE_PATH}/install")

add_compile_definitions(${MODE_MACROS})
add_compile_options(${COMPILE_OPTIONS})
add_link_options(${LINK_OPTIONS})

string(REPLACE ";" " " ACTIVATED_KEYS_LIST "${ACTIVATED_KEYS}")
function(check_keys_intersection required_keys result_var)
    set(intersection FALSE)
    if("${required_keys}" STREQUAL "InstallAlways")
        set(intersection TRUE)
    else()
        foreach(key ${required_keys})
            list(FIND ACTIVATED_KEYS_LIST "${key}" is_found)
            if(NOT is_found EQUAL -1)
                set(intersection TRUE)
                break()
            endif()
        endforeach()
    endif()
    set(${result_var} ${intersection} PARENT_SCOPE)
endfunction()

# --- Sub-projects (in-source builds) ---

# --- Dependencies (finding external packages) ---

# --- Project module definitions ---
# Module: NReflectionCore
check_keys_intersection("InstallAlways" ACTIVATE_MOD_NReflectionCore)
if(ACTIVATE_MOD_NReflectionCore)
    add_library(NReflectionCore SHARED)
    target_sources(NReflectionCore
        PRIVATE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/NReflectionCore/private/ClassBase.cpp>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/NReflectionCore/private/NReflectionCore.cpp>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/NReflectionCore/private/Type.cpp>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/NReflectionCore/private/TypeManager.cpp>
     "Source/NReflectionCore/public/TypeMeta.h" "Source/NReflectionCore/public/ClassMember.h" "Source/NReflectionCore/public/StaticMember.h" "Source/NReflectionCore/public/MemberFunctionMeta.h" "Source/NReflectionCore/public/MemberVariableMeta.h" "Source/NReflectionCore/public/StaticFunctionMeta.h" "Source/NReflectionCore/public/StaticVariableMeta.h")
    target_include_directories(NReflectionCore
        PUBLIC
            $<INSTALL_INTERFACE:include/NReflectionCore/public>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/NReflectionCore/public>
    )
    target_compile_definitions(NReflectionCore PRIVATE NReflectionCore)



    install(TARGETS NReflectionCore
        EXPORT NReflectionTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
    set(PUBLIC_INC_NREFLECTIONCORE
        "Source/NReflectionCore/public"
    )

    install(DIRECTORY 
        ${PUBLIC_INC_NREFLECTIONCORE}
        DESTINATION include/NReflectionCore
    )
endif()

# Module: NReflectionExe
check_keys_intersection("InstallAlways" ACTIVATE_MOD_NReflectionExe)
if(ACTIVATE_MOD_NReflectionExe)
    add_executable(NReflectionExe)
    target_sources(NReflectionExe
        PRIVATE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/NReflectionExe/private/NReflectionExe.cpp>
     "Source/NReflectionCore/public/TypeMeta.h" "Source/NReflectionCore/public/ClassMember.h" "Source/NReflectionCore/public/StaticMember.h" "Source/NReflectionCore/public/MemberFunctionMeta.h" "Source/NReflectionCore/public/MemberVariableMeta.h" "Source/NReflectionCore/public/StaticFunctionMeta.h" "Source/NReflectionCore/public/StaticVariableMeta.h")
    target_include_directories(NReflectionExe
        PUBLIC
            $<INSTALL_INTERFACE:include/NReflectionExe/public>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/NReflectionExe/public>
    )
    target_compile_definitions(NReflectionExe PRIVATE NReflectionExe)

    check_keys_intersection("InstallAlways" NREFLECTIONEXE_ACTIVATE_DEP_NREFLECTIONCORE)
    if(NREFLECTIONEXE_ACTIVATE_DEP_NREFLECTIONCORE)
        target_link_libraries(NReflectionExe PUBLIC NReflectionCore)
        add_dependencies(NReflectionExe NReflectionCore)
    endif()


    install(TARGETS NReflectionExe
        EXPORT NReflectionTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
    set(PUBLIC_INC_NREFLECTIONEXE
        "Source/NReflectionExe/public"
    )

    install(DIRECTORY 
        ${PUBLIC_INC_NREFLECTIONEXE}
        DESTINATION include/NReflectionExe
    )
endif()


install(EXPORT NReflectionTargets
    FILE NReflectionConfig.cmake
    NAMESPACE NReflection::
    DESTINATION lib/cmake/NReflection
)
# --- Sub-projects (in-source builds) ---
