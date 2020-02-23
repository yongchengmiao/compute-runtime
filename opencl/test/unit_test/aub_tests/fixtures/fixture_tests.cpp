/*
 * Copyright (C) 2017-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "core/command_stream/command_stream_receiver.h"
#include "core/helpers/ptr_math.h"
#include "opencl/source/mem_obj/image.h"

#include "aub_tests/command_stream/aub_command_stream_fixture.h"
#include "command_queue/command_enqueue_fixture.h"
#include "gtest/gtest.h"
#include "mocks/mock_context.h"

using namespace NEO;

struct SimpleTest : public CommandDeviceFixture,
                    public AUBCommandStreamFixture,
                    public ::testing::Test {

    using AUBCommandStreamFixture::SetUp;

    void SetUp() override {
        CommandDeviceFixture::SetUp(cl_command_queue_properties(0));
        AUBCommandStreamFixture::SetUp(pCmdQ);
        context = new MockContext(pClDevice);
    }
    void TearDown() override {
        delete context;
        AUBCommandStreamFixture::TearDown();
        CommandDeviceFixture::TearDown();
    }
    MockContext *context;
};

TEST_F(SimpleTest, VerifyBasicFixturesSetupAndTearDown) {
    EXPECT_EQ(1, 1);
}