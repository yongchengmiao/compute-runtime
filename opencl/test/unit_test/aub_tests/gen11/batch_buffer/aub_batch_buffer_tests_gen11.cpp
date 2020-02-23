/*
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "aub_batch_buffer_tests_gen11.h"

#include "fixtures/device_fixture.h"

using Gen11AubBatchBufferTests = Test<NEO::DeviceFixture>;

static constexpr auto gpuBatchBufferAddr = 0x800400001000; // 48-bit GPU address

GEN11TEST_F(Gen11AubBatchBufferTests, givenSimpleRCSWithBatchBufferWhenItHasMSBSetInGpuAddressThenAUBShouldBeSetupSuccessfully) {
    setupAUBWithBatchBuffer<FamilyType>(pDevice, aub_stream::ENGINE_RCS, gpuBatchBufferAddr);
}