#
# Copyright (C) 2017-2020 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

add_custom_target(builtins)
set_target_properties(builtins PROPERTIES FOLDER "built_ins")
set(BUILTINS_OUTDIR_WITH_ARCH "${TargetDir}/built_ins/${NEO_ARCH}")
add_dependencies(${BUILTINS_BINARIES_LIB_NAME} builtins)
add_subdirectories()
set(GENERATED_BUILTINS ${GENERATED_BUILTINS} PARENT_SCOPE)
set(GENERATED_BUILTINS_IMAGES ${GENERATED_BUILTINS_IMAGES} PARENT_SCOPE)
set(GENERATED_BUILTINS_STATELESS ${GENERATED_BUILTINS_STATELESS} PARENT_SCOPE)

if("${NEO_ARCH}" STREQUAL "x32")
  set(BUILTIN_OPTIONS "-cl-intel-greater-than-4GB-buffer-required")
else()
  set(BUILTIN_OPTIONS "")
endif()

set(BUILTIN_OPTIONS_STATELESS
    "-cl-intel-greater-than-4GB-buffer-required"
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  list(APPEND __cloc__options__ "-D DEBUG")
endif()

set(BUILTINS_INCLUDE_DIR ${TargetDir} PARENT_SCOPE)
set(BUILTIN_CPP "")

function(get_bits_for_stateless gen_type platform_type)
  # Force 32bits compiling on gen9lp for stateless builtins
  if((${GEN_TYPE} STREQUAL "GEN9") AND (${PLATFORM_TYPE} STREQUAL "LP"))
    set(BITS "32" PARENT_SCOPE)
  else()
    set(BITS ${NEO_BITS} PARENT_SCOPE)
  endif()
endfunction()

# Define function for compiling built-ins (with ocloc)
function(compile_builtin gen_type platform_type builtin bits builtin_options)
  string(TOLOWER ${gen_type} gen_type_lower)
  get_family_name_with_type(${gen_type} ${platform_type})
  set(OUTPUTDIR "${BUILTINS_OUTDIR_WITH_ARCH}/${gen_type_lower}")
  # get filename
  set(FILENAME ${builtin})
  # get name of the file w/o extension
  get_filename_component(BASENAME ${builtin} NAME_WE)

  set(OUTPUTPATH_BASE "${OUTPUTDIR}/${BASENAME}_${family_name_with_type}")
  set(OUTPUT_FILES
      ${OUTPUTPATH_BASE}.spv
      ${OUTPUTPATH_BASE}.bin
      ${OUTPUTPATH_BASE}.gen
  )
  set(OUTPUT_FILE_CPP
      ${OUTPUTPATH_BASE}.cpp
  )

  # function returns builtin cpp filename
  unset(BUILTIN_CPP)
  # set variable outside function
  set(BUILTIN_CPP built_ins/${NEO_ARCH}/${gen_type_lower}/${BASENAME}_${family_name_with_type}.cpp PARENT_SCOPE)
  if(NOT DEFINED cloc_cmd_prefix)
    if(WIN32)
      set(cloc_cmd_prefix ocloc)
    else()
      if(DEFINED NEO__IGC_LIBRARY_PATH)
        set(cloc_cmd_prefix LD_LIBRARY_PATH=${NEO__IGC_LIBRARY_PATH}:$<TARGET_FILE_DIR:ocloc_lib> $<TARGET_FILE:ocloc>)
      else()
        set(cloc_cmd_prefix LD_LIBRARY_PATH=$<TARGET_FILE_DIR:ocloc_lib> $<TARGET_FILE:ocloc>)
      endif()
    endif()
  endif()

  list(APPEND __cloc__options__ "-cl-kernel-arg-info")
  add_custom_command(
                     OUTPUT ${OUTPUT_FILES}
                     COMMAND ${cloc_cmd_prefix} -q -file ${FILENAME} -device ${DEFAULT_SUPPORTED_${gen_type}_${platform_type}_PLATFORM} ${builtin_options} -${bits} -out_dir ${OUTPUTDIR} -options "$<JOIN:${__cloc__options__}, >"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     DEPENDS ${builtin} ocloc copy_compiler_files
  )
  add_custom_command(
                     OUTPUT ${OUTPUT_FILE_CPP}
                     COMMAND $<TARGET_FILE:cpp_generate_tool> --file ${OUTPUTPATH_BASE}.gen --output ${OUTPUT_FILE_CPP} --array ${BASENAME} --platform ${family_name_with_type}
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     DEPENDS ${OUTPUTPATH_BASE}.gen $<TARGET_FILE:cpp_generate_tool>
  )
endfunction()

macro(macro_for_each_gen)
  foreach(PLATFORM_TYPE ${PLATFORM_TYPES})
    if(${GEN_TYPE}_HAS_${PLATFORM_TYPE})
      get_family_name_with_type(${GEN_TYPE} ${PLATFORM_TYPE})
      string(TOLOWER ${PLATFORM_TYPE} PLATFORM_TYPE_LOWER)
      unset(BUILTINS_COMMANDS)
      foreach(GENERATED_BUILTIN ${GENERATED_BUILTINS})
        compile_builtin(${GEN_TYPE} ${PLATFORM_TYPE} ${GENERATED_BUILTIN}.builtin_kernel ${NEO_BITS} "${BUILTIN_OPTIONS}")
        list(APPEND BUILTINS_COMMANDS ${TargetDir}/${BUILTIN_CPP})
        set(RUNTIME_GENERATED_${GENERATED_BUILTIN}_${family_name_with_type} ${BUILTIN_CPP} PARENT_SCOPE)
      endforeach()
      foreach(GENERATED_BUILTINS_IMAGES ${GENERATED_BUILTINS_IMAGES})
        compile_builtin(${GEN_TYPE} ${PLATFORM_TYPE} ${GENERATED_BUILTINS_IMAGES}.builtin_kernel ${NEO_BITS} "${BUILTIN_OPTIONS}")
        set(IMAGE_SUPPORT FALSE)
        GEN_CONTAINS_PLATFORMS("SUPPORTED_IMAGES" ${GEN_TYPE} IMAGE_SUPPORT)
        if(${IMAGE_SUPPORT})
          list(APPEND BUILTINS_COMMANDS ${TargetDir}/${BUILTIN_CPP})
          set(RUNTIME_GENERATED_${GENERATED_BUILTINS_IMAGES}_${family_name_with_type} ${BUILTIN_CPP} PARENT_SCOPE)
        endif()
      endforeach()
      get_bits_for_stateless(${GEN_TYPE} ${PLATFORM_TYPE})
      foreach(GENERATED_BUILTIN_STATELESS ${GENERATED_BUILTINS_STATELESS})
        compile_builtin(${GEN_TYPE} ${PLATFORM_TYPE} ${GENERATED_BUILTIN_STATELESS}.builtin_kernel ${BITS} "${BUILTIN_OPTIONS_STATELESS}")
        list(APPEND BUILTINS_COMMANDS ${TargetDir}/${BUILTIN_CPP})
        set(RUNTIME_GENERATED_${GENERATED_BUILTIN_STATELESS}_${family_name_with_type} ${BUILTIN_CPP} PARENT_SCOPE)
      endforeach()
      foreach(GENERATED_BUILTIN_IMAGES_STATELESS ${GENERATED_BUILTINS_IMAGES_STATELESS})
        compile_builtin(${GEN_TYPE} ${PLATFORM_TYPE} ${GENERATED_BUILTIN_IMAGES_STATELESS}.builtin_kernel ${BITS} "${BUILTIN_OPTIONS_STATELESS}")
        set(IMAGE_SUPPORT FALSE)
        GEN_CONTAINS_PLATFORMS("SUPPORTED_IMAGES" ${GEN_TYPE} IMAGE_SUPPORT)
        if(${IMAGE_SUPPORT})
          list(APPEND BUILTINS_COMMANDS ${TargetDir}/${BUILTIN_CPP})
          set(RUNTIME_GENERATED_${GENERATED_BUILTIN_IMAGES_STATELESS}_${family_name_with_type} ${BUILTIN_CPP} PARENT_SCOPE)
        endif()
      endforeach()
      set(target_name builtins_${family_name_with_type})
      add_custom_target(${target_name} DEPENDS ${BUILTINS_COMMANDS})
      add_dependencies(builtins ${target_name})
      set_target_properties(${target_name} PROPERTIES FOLDER "built_ins/${family_name_with_type}")
    endif()
  endforeach()
endmacro()

apply_macro_for_each_gen("SUPPORTED")
