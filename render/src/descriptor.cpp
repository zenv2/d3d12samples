#include "utils.h"
#include <descriptor.h>

namespace render
{
    Descriptor::Descriptor(Device& dev) :
        m_device(dev),
        m_pRootSignature(nullptr),
        m_nParams(0)
    {
        CreateRootSignature();
    }

    void Descriptor::CreateRootSignature()
    {
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.NumParameters = 0;
        rootSignatureDesc.pParameters = nullptr;
        rootSignatureDesc.NumStaticSamplers = 0;
        rootSignatureDesc.pStaticSamplers = nullptr;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        CreateRootSignature(&rootSignatureDesc);
    }

    Descriptor::~Descriptor()
    {
        SafeRelease(m_pRootSignature);
    }

    void Descriptor::CreateRootSignature(D3D12_ROOT_SIGNATURE_DESC* desc)
    {
        if(m_pRootSignature)
        {
            SafeRelease(m_pRootSignature);
            m_pRootSignature = nullptr;
        }

        ID3DBlob* signature;
        ID3DBlob** pError = nullptr;
#if defined(_DEBUG)
        ID3DBlob* error = nullptr;
        pError = &error;
#endif
        ThrowIfFailed(D3D12SerializeRootSignature(desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, pError));
#if defined(_DEBUG)            
        if(error)
        {
            //Todo: error->Release() missing?
            std::runtime_error((const char*)error->GetBufferPointer());
        }
#endif
        ThrowIfFailed(m_device.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)));
    }
}