#include <buffer.h>
#include "utils.h"

namespace render
{
    Buffer::Buffer(Device& device, D3D12_HEAP_TYPE heapType, uint32_t bufferSize) :
        m_bufferSize(bufferSize)
    {
        D3D12_HEAP_PROPERTIES heapProps;
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC resourceDesc;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = m_bufferSize;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        ThrowIfFailed(device.GetDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_pBuffer)));
    }

    Buffer::~Buffer()
    {
        SafeRelease(m_pBuffer);
    }

    VertexBuffer::VertexBuffer(Device& device, void* pBuff, uint32_t stride, uint32_t sizeInBytes) :
        m_stride(stride),
        Buffer(device, D3D12_HEAP_TYPE_UPLOAD, sizeInBytes)
    {
        Init(pBuff);
    }

    VertexBuffer::~VertexBuffer()
    {

    }

    void VertexBuffer::Init(void* pBuff)
    {
        uint8_t* pVertexDataBegin;
        D3D12_RANGE readRange;
        readRange.Begin = 0;
        readRange.End = 0;

        ThrowIfFailed(GetBuffer()->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));

        memcpy(pVertexDataBegin, pBuff, m_bufferSize);

        GetBuffer()->Unmap(0, nullptr);

        m_view.BufferLocation = GetBuffer()->GetGPUVirtualAddress();
        m_view.StrideInBytes = m_stride;
        m_view.SizeInBytes = m_bufferSize;
    }

    IndexBuffer::IndexBuffer(Device& device, void* pBuff, uint32_t sizeInBytes) :
        Buffer(device, D3D12_HEAP_TYPE_UPLOAD, sizeInBytes)
    {
        Init(pBuff);
    }

    IndexBuffer::~IndexBuffer()
    {

    }

    void IndexBuffer::Init(void* pBuff)
    {
        uint8_t* pVertexDataBegin;
        D3D12_RANGE readRange;
        readRange.Begin = 0;
        readRange.End = 0;

        ThrowIfFailed(GetBuffer()->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));

        memcpy(pVertexDataBegin, pBuff, m_bufferSize);

        GetBuffer()->Unmap(0, nullptr);

        m_view.BufferLocation = GetBuffer()->GetGPUVirtualAddress();
        m_view.Format = DXGI_FORMAT_R32_UINT;
        m_view.SizeInBytes = m_bufferSize;
    }
}