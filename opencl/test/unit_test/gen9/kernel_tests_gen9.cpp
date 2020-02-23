/*
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "opencl/source/helpers/hardware_commands_helper.h"
#include "opencl/source/mem_obj/buffer.h"
#include "test.h"

#include "fixtures/device_fixture.h"
#include "mocks/mock_kernel.h"

using namespace NEO;

using Gen9KernelTest = Test<DeviceFixture>;
GEN9TEST_F(Gen9KernelTest, givenKernelWhenCanTransformImagesIsCalledThenReturnsTrue) {
    MockKernelWithInternals mockKernel(*pClDevice);
    auto retVal = mockKernel.mockKernel->Kernel::canTransformImages();
    EXPECT_TRUE(retVal);
}
using Gen9HardwareCommandsTest = testing::Test;
GEN9TEST_F(Gen9HardwareCommandsTest, givenGen9PlatformWhenDoBindingTablePrefetchIsCalledThenReturnsTrue) {
    EXPECT_TRUE(HardwareCommandsHelper<FamilyType>::doBindingTablePrefetch());
}