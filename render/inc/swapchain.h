#pragma once

#include <stdint.h>
#include <windows.h>
#include <window.h>
#include <commander.h>

namespace render
{
    class Swapchain
    {
    public:
        Swapchain(const Window& window, Device& device, Commander& commander);
        ~Swapchain();

        void UpdateFrameIndex() { m_frameIndex = m_swapchain->GetCurrentBackBufferIndex(); }
        void TransitionBarrierPresentToRenderTarget(D3D12_RESOURCE_BARRIER& barrier);
        void TransitionBarrierRenderTargetToPresent(D3D12_RESOURCE_BARRIER& barrier);
        D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle();
        void Present();

        friend class Pipeline;
    
    protected:
        DXGI_FORMAT GetFormat() { return FORMAT; }

    private:
        static const uint32_t FRAME_COUNT = 2;
        static const DXGI_FORMAT FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;
        
        IDXGISwapChain3* m_swapchain;
        ID3D12DescriptorHeap* m_rtvHeap;
        ID3D12Resource* m_renderTargets[FRAME_COUNT];

        uint32_t m_frameIndex;
        uint32_t m_rtvDescriptorSize;
    };
}