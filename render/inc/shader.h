#pragma once

#include <exception>

#include <d3dcompiler.h>
#include <d3d12.h>
#include <windows.h>

#include <device.h>

namespace render
{
    enum SHADER_TYPE
    {
        SHADER_TYPE_VERTEX,
        SHADER_TYPE_PIXEL,
        SHADER_TYPE_MAX
    };

    // Caution: order matters!
    const char targets[SHADER_TYPE_MAX][10] =
    {
        "vs_5_1",
        "ps_5_1"
    };
    
    template<SHADER_TYPE _type>
    class Shader
    {
    public:
        Shader(const char* buf, const size_t length, const char* entryPoint)
        {
            if( buf == nullptr ||
                length == 0 ||
                entryPoint == nullptr )
            {
                throw std::exception("Bad Shader Code!");
            }

            uint32_t flags = 0;
            ID3DBlob** pError = nullptr;
#if defined(_DEBUG)
            flags =  D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
            ID3DBlob* error = nullptr;
            pError = &error;
#endif
            D3DCompile(buf, length, nullptr, nullptr, nullptr, entryPoint, targets[_type], flags, 0, &m_pCode, pError);
#if defined(_DEBUG)            
            if(error)
            {
                throw std::exception((const char*)error->GetBufferPointer());
            }
#endif
        }

        ~Shader()
        {
            if(m_pCode) m_pCode->Release();
        }
    
        friend class Pipeline;
    protected:
        LPVOID GetCode() { return m_pCode->GetBufferPointer(); }
        SIZE_T GetSize() { return m_pCode->GetBufferSize(); }

    private:
        ID3DBlob* m_pCode;
    };

    using VertexShader = Shader<SHADER_TYPE::SHADER_TYPE_VERTEX>;
    using PixelShader = Shader<SHADER_TYPE::SHADER_TYPE_PIXEL>;
}

