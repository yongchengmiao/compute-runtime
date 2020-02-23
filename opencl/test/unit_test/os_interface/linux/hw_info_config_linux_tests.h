/*
 * Copyright (C) 2017-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "core/os_interface/hw_info_config.h"
#include "core/os_interface/os_interface.h"
#include "core/utilities/cpu_info.h"

#include "os_interface/hw_info_config_tests.h"
#include "os_interface/linux/drm_mock.h"

using namespace NEO;
using namespace std;

struct HwInfoConfigTestLinux : public HwInfoConfigTest {
    void SetUp() override;
    void TearDown() override;

    OSInterface *osInterface;

    DrmMock *drm;

    void (*rt_cpuidex_func)(int *, int, int);
};