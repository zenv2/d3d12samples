#pragma once

#include <windows.h>
#include <d3d12.h>

#include <device.h>

namespace render
{
    class Descriptor
    {
    public:
        Descriptor(Device& dev);
        ~Descriptor();
    
        friend class Pipeline;
    
    protected:
        ID3D12RootSignature* GetRootSignature() { return m_pRootSignature; }

    private:
        Device& m_device;

        ID3D12RootSignature* m_pRootSignature;
    };
}