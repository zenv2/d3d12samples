#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>

namespace render
{
    class Device
    {
    public:

        Device();
        ~Device();

        friend class Commander;
        friend class Swapchain;
        friend class Descriptor;
        friend class Pipeline;
        friend class Buffer;
    protected:
        IDXGIFactory4* GetFactory() const { return m_pFactory; }
        ID3D12Device* GetDevice() { return m_pDevice; }

    private:
        IDXGIFactory4 *m_pFactory;
#if defined(_DEBUG)
        ID3D12Debug1* m_pDebugController;
        ID3D12DebugDevice* m_pDebugDevice;
#endif
        IDXGIAdapter1* m_pAdapter;
        ID3D12Device* m_pDevice;
    };
}