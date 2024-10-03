#include "utils.h"
#include <commander.h>
#include <swapchain.h>

namespace render
{
    Commander::Commander(Device& dev) :
        m_pDevice(dev),
        m_pQueue(nullptr),
        m_pAllocator(nullptr),
        m_fence(nullptr),
        m_frameIndex(0),
        m_fenceEvent(nullptr),
        m_fenceValue(0)
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        ThrowIfFailed(m_pDevice.GetDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue)));

        ThrowIfFailed(m_pDevice.GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pAllocator)));

        ThrowIfFailed(m_pDevice.GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pAllocator, nullptr, IID_PPV_ARGS(&m_pCommandList)));
        m_pCommandList->Close();

        ThrowIfFailed(m_pDevice.GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if(m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    Commander::~Commander()
    {
        SafeRelease(m_fence);
        SafeRelease(m_pCommandList);
        SafeRelease(m_pAllocator);
        SafeRelease(m_pQueue);
    }

    void Commander::Reset(Pipeline *pPipeline)
    {
        ThrowIfFailed(m_pAllocator->Reset());
        ThrowIfFailed(m_pCommandList->Reset(m_pAllocator, pPipeline->GetState()));
    }

    void Commander::WaitForPreviousFrame()
    {
        const UINT64 fence = m_fenceValue;

        ThrowIfFailed(m_pQueue->Signal(m_fence, fence));
        m_fenceValue++;

        if(m_fence->GetCompletedValue() < fence)
        {
            ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

    void Commander::Execute()
    {
        ID3D12CommandList* ppCommandLists[] = { m_pCommandList };

        m_pQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}