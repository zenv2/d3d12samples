#if 0
#include "utils.h"
#include <d3dcompiler.h>
#include <shader.h>

namespace render
{
    template <SHADER_TYPE _type>
    Shader<_type>::Shader(const char* buf, const size_t length, const char* entryPoint)
    {
        if( buf == nullptr ||
            length == 0 ||
            entryPoint == nullptr )
        {
            std::runtime_error("Bad Shader Code!");
        }

        uint32_t flags = 0;
        ID3DBlob** pError = nullptr;
#if defined(_DEBUG)
        flags =  D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        ID3DBlob* error;
        pError = &error;

        try
        {
#endif
        ThrowIfFailed(D3DCompile(buf, length, nullptr, nullptr, nullptr, entryPoint, targets[_type], flags, 0, &m_pCode, pError));
#if defined(_DEBUG)            
        }
        catch (std::exception e)
        {
            //Todo: error->Release() missing?
            std::runtime_error((const char*)error->GetBufferPointer());
        }
#endif
    }

    template <SHADER_TYPE _type> Shader<_type>::~Shader()
    {
        SafeRelease(m_pCode);
    }
}

#endif