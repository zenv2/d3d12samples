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
        Swapchain(const Window& window, Device& device, Commander& commander, const DXGI_FORMAT rtvFormat, const DXGI_FORMAT dsvFormat);
        ~Swapchain();

        void CreateDepthStencil(DXGI_FORMAT fmt);
        void UpdateFrameIndex() { m_frameIndex = m_swapchain->GetCurrentBackBufferIndex(); }
        void TransitionBarrierPresentToRenderTarget(D3D12_RESOURCE_BARRIER& barrier);
        void TransitionBarrierRenderTargetToPresent(D3D12_RESOURCE_BARRIER& barrier);
        D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle();
        D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle();
        void Present();

        friend class Pipeline;
    
    protected:
        DXGI_FORMAT GetRtvFormat() const { return m_rtvFormat; }
        DXGI_FORMAT GetDsvFormat() const { return m_dsvFormat; }

    private:
        uint32_t m_width;
        uint32_t m_height;
        static const uint32_t m_frameCount = 2;
        DXGI_FORMAT m_rtvFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        DXGI_FORMAT m_dsvFormat = DXGI_FORMAT_UNKNOWN;
        
        IDXGISwapChain3* m_swapchain;
        ID3D12DescriptorHeap* m_rtvHeap;
        ID3D12DescriptorHeap* m_dsvHeap;
        ID3D12Resource* m_renderTargets[m_frameCount];
        ID3D12Resource* m_depthStencilBuffer;

        uint32_t m_frameIndex;
        uint32_t m_rtvDescriptorSize;
    };
}