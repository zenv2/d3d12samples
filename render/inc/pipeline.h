#pragma once

#include <windows.h>
#include <d3d12.h>

#include <device.h>
#include <descriptor.h>
#include <shader.h>

namespace render
{
    class Pipeline
    {
    public:
        Pipeline();
        ~Pipeline();

        void SetInputLayout(D3D12_INPUT_ELEMENT_DESC* pInputLayout, int numElements);
        void SetDescriptors(Descriptor& descriptor);
        void SetVS(VertexShader& vs);
        void SetPS(PixelShader& ps);
        void Finalize(Device& device);

    private:
        void Init();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
        D3D12_RASTERIZER_DESC m_rasterizerDesc;
        D3D12_BLEND_DESC m_blendDesc;

        ID3D12PipelineState* m_pPipelineState;
        ID3DBlob* m_pVertexShader;
        ID3DBlob* m_pPixelShader;
    };
}