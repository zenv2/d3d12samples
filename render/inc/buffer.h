#pragma once

#include <stdint.h>

#include <d3d12.h>

#include <device.h>

namespace render
{
    class Buffer
    {
        public:

        Buffer(Device& device, D3D12_HEAP_TYPE heapType, uint32_t bufferSize);
        ~Buffer();

        protected:
        uint32_t m_bufferSize = 0;

        ID3D12Resource* GetBuffer() const { return m_pBuffer; }
        private:
        ID3D12Resource* m_pBuffer;
    };

    class VertexBuffer : public Buffer
    {
        public:

        VertexBuffer(Device& device, void* pBuff, uint32_t stride, uint32_t sizeInBytes);
        ~VertexBuffer();

        const D3D12_VERTEX_BUFFER_VIEW* GetView() const { return &m_view; }

        private:
        uint32_t m_stride;
        D3D12_VERTEX_BUFFER_VIEW m_view;

        void Init(void* pBuff);
    };

    class IndexBuffer : public Buffer
    {
        public:

        IndexBuffer(Device& device, void* pBuff, uint32_t sizeInBytes);
        ~IndexBuffer();

        const D3D12_INDEX_BUFFER_VIEW* GetView() const { return &m_view; }

        private:
        D3D12_INDEX_BUFFER_VIEW m_view;

        void Init(void* pBuff);
    };
}