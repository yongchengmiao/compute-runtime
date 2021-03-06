/*
 * Copyright (C) 2019-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/memory_manager/graphics_allocation.h"

#include "opencl/source/builtin_kernels_simulation/opencl_c.h"
#include "opencl/source/builtin_kernels_simulation/scheduler_simulation.h"
#include "opencl/source/builtin_kernels_simulation/scheduler_simulation.inl"
#include "opencl/source/execution_model/device_enqueue.h"

#include "CL/cl.h"
#include "hw_cmds.h"

#include <type_traits>

using namespace NEO;
using namespace BuiltinKernelsSimulation;

namespace Gen12LPSchedulerSimulation {

#define SCHEDULER_EMULATION

uint GetNextPowerof2(uint number);

float __intel__getProfilingTimerResolution() {
    return static_cast<float>(DEFAULT_GEN12LP_PLATFORM::hwInfo.capabilityTable.defaultProfilingTimerResolution);
}

#include "opencl/source/gen12lp/device_enqueue.h"
#include "opencl/source/gen12lp/scheduler_builtin_kernel.inl"
#include "opencl/source/scheduler/scheduler.cl"
} // namespace Gen12LPSchedulerSimulation

namespace BuiltinKernelsSimulation {

template <>
void SchedulerSimulation<TGLLPFamily>::startScheduler(uint32_t index,
                                                      GraphicsAllocation *queue,
                                                      GraphicsAllocation *commandsStack,
                                                      GraphicsAllocation *eventsPool,
                                                      GraphicsAllocation *secondaryBatchBuffer,
                                                      GraphicsAllocation *dsh,
                                                      GraphicsAllocation *reflectionSurface,
                                                      GraphicsAllocation *queueStorageBuffer,
                                                      GraphicsAllocation *ssh,
                                                      GraphicsAllocation *debugQueue) {

    threadIDToLocalIDmap.insert(std::make_pair(std::this_thread::get_id(), index));

    while (!conditionReady) {
    }

    Gen12LPSchedulerSimulation::SchedulerParallel20((IGIL_CommandQueue *)queue->getUnderlyingBuffer(),
                                                    (uint *)commandsStack->getUnderlyingBuffer(),
                                                    (IGIL_EventPool *)eventsPool->getUnderlyingBuffer(),
                                                    (uint *)secondaryBatchBuffer->getUnderlyingBuffer(),
                                                    (char *)dsh->getUnderlyingBuffer(),
                                                    (IGIL_KernelDataHeader *)reflectionSurface->getUnderlyingBuffer(),
                                                    (uint *)queueStorageBuffer->getUnderlyingBuffer(),
                                                    (char *)ssh->getUnderlyingBuffer(),
                                                    debugQueue != nullptr ? (DebugDataBuffer *)debugQueue->getUnderlyingBuffer() : nullptr);
}

template <>
void SchedulerSimulation<TGLLPFamily>::patchGpGpuWalker(uint secondLevelBatchOffset,
                                                        __global uint *secondaryBatchBuffer,
                                                        uint interfaceDescriptorOffset,
                                                        uint simdSize,
                                                        uint totalLocalWorkSize,
                                                        uint3 dimSize,
                                                        uint3 startPoint,
                                                        uint numberOfHwThreadsPerWg,
                                                        uint indirectPayloadSize,
                                                        uint ioHoffset) {
    Gen12LPSchedulerSimulation::patchGpGpuWalker(secondLevelBatchOffset,
                                                 secondaryBatchBuffer,
                                                 interfaceDescriptorOffset,
                                                 simdSize,
                                                 totalLocalWorkSize,
                                                 dimSize,
                                                 startPoint,
                                                 numberOfHwThreadsPerWg,
                                                 indirectPayloadSize,
                                                 ioHoffset);
}

template class SchedulerSimulation<TGLLPFamily>;

} // namespace BuiltinKernelsSimulation
