#include "utils.h"
#include <device.h>

namespace render
{
    Device::Device() :
        m_pFactory(nullptr),
#if defined(_DEBUG)
        m_pDebugController(nullptr),
        m_pDebugDevice(nullptr),
#endif
        m_pAdapter(nullptr),
        m_pDevice(nullptr)
    {
        UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
        ID3D12Debug* dc;

        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&dc)));
        ThrowIfFailed(dc->QueryInterface(IID_PPV_ARGS(&m_pDebugController)));

        m_pDebugController->EnableDebugLayer();
        m_pDebugController->SetEnableGPUBasedValidation(true);

        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

        dc->Release();
#endif

        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_pFactory)));

        for(int adapterIdx=0; m_pFactory->EnumAdapters1(adapterIdx, &m_pAdapter) != DXGI_ERROR_NOT_FOUND; adapterIdx++)
        {
            DXGI_ADAPTER_DESC1 desc;
            m_pAdapter->GetDesc1(&desc);

            if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            if(SUCCEEDED(D3D12CreateDevice(m_pAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }

            m_pAdapter->Release();
        }

        ThrowIfFailed(D3D12CreateDevice(m_pAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice)));

#if defined(_DEBUG)
        ThrowIfFailed(m_pDevice->QueryInterface(&m_pDebugDevice));
#endif        
    }

    Device::~Device()
    {
#if defined(_DEBUG)
        SafeRelease(m_pDebugDevice);
#endif
        SafeRelease(m_pDevice);
        SafeRelease(m_pAdapter);
#if defined(_DEBUG)
        SafeRelease(m_pDebugController);
#endif
        SafeRelease(m_pFactory);
    }
}