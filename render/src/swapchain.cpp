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
        desc.Width = window.GetWidth();
        desc.Height = window.GetHeight();
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

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.NumDescriptors = FRAME_COUNT;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ThrowIfFailed(device.GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        m_rtvDescriptorSize = device.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for(int i=0; i < FRAME_COUNT; i++)
        {
            ThrowIfFailed(m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
            device.GetDevice()->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);
            rtvHandle.ptr += m_rtvDescriptorSize;
        }

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ThrowIfFailed(device.GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC resourceDesc {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = window.GetWidth();
        resourceDesc.Height = window.GetHeight();
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        ThrowIfFailed(device.GetDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&m_depthStencilBuffer)));
        
        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        device.GetDevice()->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
    }

    Swapchain::~Swapchain()
    {
        SafeRelease(m_depthStencilBuffer);
        SafeRelease(m_dsvHeap);
        SafeRelease(m_rtvHeap);
        SafeRelease(m_swapchain);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Swapchain::GetRtvHandle()
    {
        auto handle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += (m_frameIndex * m_rtvDescriptorSize);
        return handle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Swapchain::GetDsvHandle()
    {
        return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    }

    void Swapchain::TransitionBarrierPresentToRenderTarget(D3D12_RESOURCE_BARRIER& barrier)
    {
        ZeroMemory(&barrier, sizeof(D3D12_RESOURCE_BARRIER));
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_renderTargets[m_frameIndex];
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    }

    void Swapchain::TransitionBarrierRenderTargetToPresent(D3D12_RESOURCE_BARRIER& barrier)
    {
        ZeroMemory(&barrier, sizeof(D3D12_RESOURCE_BARRIER));
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_renderTargets[m_frameIndex];
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    }

    void Swapchain::Present()
    {
        ThrowIfFailed(m_swapchain->Present(1, 0));
    }
}