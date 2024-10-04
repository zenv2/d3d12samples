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
    
        ID3D12RootSignature* GetRootSignature() { return m_pRootSignature; }
   
    protected:

    private:
        Device& m_device;

        ID3D12RootSignature* m_pRootSignature;
    };
}