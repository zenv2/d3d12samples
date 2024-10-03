#pragma once

#include <d3d12.h>

#include <device.h>
#include <pipeline.h>

namespace render
{
    class Commander
    {
    public:
        Commander(Device& dev);

        ~Commander();

        void Reset(Pipeline* pPipeline);
        void WaitForPreviousFrame();
        ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList; }
        void Execute();

        friend class Swapchain;

    protected:
        ID3D12CommandQueue* GetCommandQueue() { return m_pQueue; }

    private:
        Device& m_pDevice;
        ID3D12CommandQueue* m_pQueue;
        ID3D12CommandAllocator* m_pAllocator;
        ID3D12GraphicsCommandList* m_pCommandList;

        UINT m_frameIndex;
        HANDLE m_fenceEvent;
        ID3D12Fence* m_fence;
        UINT64 m_fenceValue;
    };
}