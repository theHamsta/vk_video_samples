/*
 * Copyright (C) 2016 Google, Inc.
 * Copyright 2022 NVIDIA Corporation.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SHELL_H
#define SHELL_H

#include <queue>
#include <vector>
#include <stdexcept>
#include <atomic>
#include <vulkan_interfaces.h>

#include "VkVideoCore/VkVideoRefCountBase.h"
#include "VkCodecUtils/FrameProcessor.h"
#include "VkCodecUtils/ProgramConfig.h"
#include "VkCodecUtils/VulkanDeviceContext.h"
#include "VkShell/VkWsiDisplay.h"

static VkSemaphore vkNullSemaphore = VkSemaphore(0);

class FrameProcessor;

class Shell : public VkWsiDisplay, public VkVideoRefCountBase {
public:
    Shell(const Shell &sh) = delete;
    Shell &operator=(const Shell &sh) = delete;

    static const std::vector<VkExtensionProperties>& GetRequiredInstanceExtensions(bool directToDisplayMode);
    static VkResult Create(const VulkanDeviceContext* vkDevCtx,
                           VkSharedBaseObj<FrameProcessor>& frameProcessor,
                           bool directToDisplayMode,
                           VkSharedBaseObj<Shell>& displayShell);

    virtual int32_t AddRef()
    {
        return ++m_refCount;
    }

    virtual int32_t Release()
    {
        uint32_t ret = --m_refCount;
        // Destroy the device if ref-count reaches zero
        if (ret == 0) {
            delete this;
        }
        return ret;
    }

    typedef enum BACK_BUFFER_STATE { BACK_BUFFER_INIT, BACK_BUFFER_PREPARE, BACK_BUFFER_IN_SWAPCHAIN, BACK_BUFFER_CANCELED } BACK_BUFFER_STATE;

    struct AcquireBuffer {

        AcquireBuffer();
        ~AcquireBuffer();
        VkResult Create(const VulkanDeviceContext* vkDevCtx);

        const VulkanDeviceContext* m_vkDevCtx;
        VkSemaphore                m_semaphore;
        VkFence                    m_fence;
    };

    class BackBuffer {

    public:
        BackBuffer();
        ~BackBuffer();
        VkResult Create(const VulkanDeviceContext* vkDevCtx);


        AcquireBuffer* SetAcquireBuffer(uint32_t imageIndex, AcquireBuffer* acquireBuffer) {
            AcquireBuffer* oldAcquireBuffer = m_acquireBuffer;
            m_imageIndex = imageIndex;
            m_acquireBuffer = acquireBuffer;
            return oldAcquireBuffer;
        }

        const VkSemaphore& GetAcquireSemaphore() const {
            if (m_acquireBuffer){
                return m_acquireBuffer->m_semaphore;
            } else {
                return vkNullSemaphore;
            }
        }

        const VkSemaphore& GetRenderSemaphore() const {
            return m_renderSemaphore;
        }

        uint32_t GetImageIndex() const {
            return m_imageIndex;
        }

    private:
        const VulkanDeviceContext* m_vkDevCtx;
        uint32_t                   m_imageIndex;

        AcquireBuffer*             m_acquireBuffer;
        VkSemaphore                m_renderSemaphore;
    };

    struct Context {

        Context(const VulkanDeviceContext* vkDevCtx)
        : devCtx(vkDevCtx)
        , acquireBuffers()
        , backBuffers()
        , currentBackBuffer()
        , surface()
        , format()
        , swapchain()
        , extent()
        , acquiredFrameId() {}

        const VulkanDeviceContext* devCtx;

        std::queue<AcquireBuffer*> acquireBuffers;
        std::vector<BackBuffer>    backBuffers;
        uint32_t                   currentBackBuffer;

        VkSurfaceKHR surface;
        VkSurfaceFormatKHR format;

        VkSwapchainKHR swapchain;
        VkExtent2D extent;

        uint64_t acquiredFrameId;
    };
    const Context &GetContext() const { return m_ctx; }

    const BackBuffer& GetCurrentBackBuffer() const {
        return m_ctx.backBuffers[m_ctx.currentBackBuffer];
    }

    enum LogPriority {
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARN,
        LOG_ERR,
    };

    virtual void Log(LogPriority priority, const char *msg);

    virtual void RunLoop() = 0;
    virtual void QuitLoop() = 0;

private:
    std::atomic<int32_t>       m_refCount;
protected:
    Shell(const VulkanDeviceContext* devCtx, VkSharedBaseObj<FrameProcessor>& frameProcessor);
    virtual ~Shell() {}

    void CreateContext();
    void DestroyContext();

    void ResizeSwapchain(uint32_t width_hint, uint32_t height_hint);

    void AcquireBackBuffer(bool trainFrame = false);
    void PresentBackBuffer(bool trainFrame = false);

    VkSharedBaseObj<FrameProcessor> m_frameProcessor;
    const ProgramConfig &m_settings;

private:

    // called by create_context
    void CreateBackBuffers();
    void DestroyBackBuffers();
    virtual VkSurfaceKHR CreateSurface(VkInstance instance) = 0;
    void CreateSwapchain();
    void DestroySwapchain();

protected:
    void FakePresent();
    Context m_ctx;
private:
    const float m_tick;
    float       m_time;
};

#endif  // SHELL_H
