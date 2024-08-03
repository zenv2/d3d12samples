#include "utils.h"
#include <commander.h>

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

        ThrowIfFailed(m_pDevice.GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    }

    Commander::~Commander()
    {
        SafeRelease(m_fence);
        SafeRelease(m_pAllocator);
        SafeRelease(m_pQueue);
    }
}