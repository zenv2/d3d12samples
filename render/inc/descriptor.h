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
    
        void CreateRootSignature();
        void CreateRootSignature(D3D12_ROOT_SIGNATURE_DESC* desc);
        ID3D12RootSignature* GetRootSignature() { return m_pRootSignature; }
   
    protected:

    private:
        Device& m_device;

        uint32_t m_nParams;

        ID3D12RootSignature* m_pRootSignature;
    };
}