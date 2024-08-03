#include "utils.h"
#include <swapchain.h>

namespace render
{
    Swapchain::Swapchain(const Window& window, Device& device, Commander& commander) :
        m_swapchain(nullptr),
        m_rtvHeap(nullptr),
        m_frameIndex(0),
        m_rtvDescriptorSize(0)
    {
        for(int i=0; i<FRAME_COUNT; i++)
        {
            m_renderTargets[i] = nullptr;
        }

        IDXGISwapChain1* swapchain;

        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.BufferCount = FRAME_COUNT;
        desc.Width = static_cast<UINT>(window.GetWidth());
        desc.Height = static_cast<UINT>(window.GetHeight());
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc.Count = 1;

        ThrowIfFailed(
            device.GetFactory()->CreateSwapChainForHwnd(
                commander.GetCommandQueue(),
                window.GetWindowHandle(),
                &desc,
                nullptr,
                nullptr,
                &swapchain));

        ThrowIfFailed(device.GetFactory()->MakeWindowAssociation(window.GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER));

        ThrowIfFailed(swapchain->QueryInterface(IID_PPV_ARGS(&m_swapchain)));

        m_frameIndex = m_swapchain->GetCurrentBackBufferIndex();

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.NumDescriptors = FRAME_COUNT;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ThrowIfFailed(device.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        m_rtvDescriptorSize = device.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for(int i=0; i < FRAME_COUNT; i++)
        {
            ThrowIfFailed(m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
            device.GetDevice()->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);
            rtvHandle.ptr += m_rtvDescriptorSize;
        }
    }

    Swapchain::~Swapchain()
    {
        SafeRelease(m_rtvHeap);
        SafeRelease(m_swapchain);
    }
}