#
# Copyright (C) 2020 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

add_subdirectory(core)
add_subdirectory(experimental${BRANCH_DIR_SUFFIX}/)
add_subdirectory(experimental/tracing)
add_subdirectory(tools)
add_subdirectory(sysman)

set(L0_API "")
append_sources_from_properties(L0_API
                               L0_SRCS_API
                               L0_EXPERIMENTAL_API
                               L0_EXPERIMENTAL_TRACING_API
                               L0_SYSMAN_API
                               L0_TOOLS_API
)
set_property(GLOBAL PROPERTY L0_API ${L0_API})
