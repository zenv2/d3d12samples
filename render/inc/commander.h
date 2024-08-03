#pragma once

#include <d3d12.h>

#include <device.h>

namespace render
{
    class Commander
    {
    public:
        Commander(Device& dev);

        ~Commander();

        friend class Swapchain;

    protected:
        ID3D12CommandQueue* GetCommandQueue() { return m_pQueue; }

    private:
        Device& m_pDevice;
        ID3D12CommandQueue* m_pQueue;
        ID3D12CommandAllocator* m_pAllocator;

        UINT m_frameIndex;
        HANDLE m_fenceEvent;
        ID3D12Fence* m_fence;
        UINT64 m_fenceValue;
    };
}