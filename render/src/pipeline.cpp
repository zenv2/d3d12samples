#include "utils.h"

#include <d3dcompiler.h>

#include <pipeline.h>

#include <swapchain.h>

namespace render
{
    Pipeline::Pipeline(Swapchain& swapchain) :
        m_pVertexShader(nullptr),
        m_pPixelShader(nullptr),
        m_pPipelineState(nullptr)
    {
        Init(swapchain);
    }

    Pipeline::~Pipeline()
    {
        SafeRelease(m_pVertexShader);
        SafeRelease(m_pPixelShader);
        SafeRelease(m_pPipelineState);
    }

    void Pipeline::Init(Swapchain& swapchain)
    {
        ZeroMemory(&m_psoDesc, sizeof(m_psoDesc));
        ZeroMemory(&m_rasterizerDesc, sizeof(m_rasterizerDesc));
        ZeroMemory(&m_blendDesc, sizeof(m_blendDesc));
        ZeroMemory(&m_depthStencilDesc, sizeof(m_depthStencilDesc));

        m_rasterizerDesc.FillMode               = D3D12_FILL_MODE_SOLID;
        m_rasterizerDesc.CullMode               = D3D12_CULL_MODE_BACK;
        m_rasterizerDesc.FrontCounterClockwise  = FALSE;
        m_rasterizerDesc.DepthBias              = D3D12_DEFAULT_DEPTH_BIAS;
        m_rasterizerDesc.DepthBiasClamp         = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        m_rasterizerDesc.SlopeScaledDepthBias   = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        m_rasterizerDesc.DepthClipEnable        = TRUE;
        m_rasterizerDesc.MultisampleEnable      = FALSE;
        m_rasterizerDesc.AntialiasedLineEnable  = FALSE;
        m_rasterizerDesc.ForcedSampleCount      = 0;
        m_rasterizerDesc.ConservativeRaster     = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        
        m_blendDesc.AlphaToCoverageEnable                   = FALSE;
        m_blendDesc.IndependentBlendEnable                  = FALSE;
        m_blendDesc.RenderTarget[0].BlendEnable             = FALSE;
        m_blendDesc.RenderTarget[0].LogicOpEnable           = FALSE;
        m_blendDesc.RenderTarget[0].SrcBlend                = D3D12_BLEND_ONE;
        m_blendDesc.RenderTarget[0].DestBlend               = D3D12_BLEND_ZERO;
        m_blendDesc.RenderTarget[0].BlendOp                 = D3D12_BLEND_OP_ADD;
        m_blendDesc.RenderTarget[0].SrcBlendAlpha           = D3D12_BLEND_ONE;
        m_blendDesc.RenderTarget[0].DestBlendAlpha          = D3D12_BLEND_ZERO;
        m_blendDesc.RenderTarget[0].BlendOpAlpha            = D3D12_BLEND_OP_ADD;
        m_blendDesc.RenderTarget[0].LogicOp                 = D3D12_LOGIC_OP_NOOP;
        m_blendDesc.RenderTarget[0].RenderTargetWriteMask   = D3D12_COLOR_WRITE_ENABLE_ALL;

        
        m_psoDesc.RasterizerState = m_rasterizerDesc;
        m_psoDesc.BlendState = m_blendDesc;


        m_psoDesc.SampleMask = UINT_MAX;
        m_psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        m_psoDesc.NumRenderTargets = 1;
        //Todo: Get from Swapchain
        m_psoDesc.RTVFormats[0] = swapchain.GetRtvFormat();
        
        m_psoDesc.SampleDesc.Count = 1;

        if(swapchain.GetDsvFormat() != DXGI_FORMAT_UNKNOWN)
        {
            m_depthStencilDesc.DepthEnable = TRUE;
            m_depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            m_depthStencilDesc.StencilEnable = FALSE;
            m_depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
            m_depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
            const D3D12_DEPTH_STENCILOP_DESC defaultStencilOpDesc = {D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS};
            m_depthStencilDesc.FrontFace = defaultStencilOpDesc;
            m_depthStencilDesc.BackFace = defaultStencilOpDesc;

            m_psoDesc.DepthStencilState = m_depthStencilDesc;

            m_psoDesc.DSVFormat = swapchain.GetDsvFormat();
        }
    }

    void Pipeline::SetInputLayout(D3D12_INPUT_ELEMENT_DESC* pInputLayout, int numElements)
    {
        m_psoDesc.InputLayout.pInputElementDescs = pInputLayout;
        m_psoDesc.InputLayout.NumElements = numElements;
    }

    void Pipeline::SetDescriptors(Descriptor& descriptor)
    {
        m_psoDesc.pRootSignature = descriptor.GetRootSignature();
    }

    void Pipeline::SetVS(VertexShader& vs)
    {
        m_psoDesc.VS.pShaderBytecode = vs.GetCode();
        m_psoDesc.VS.BytecodeLength = vs.GetSize();
    }

    void Pipeline::SetPS(PixelShader& ps)
    {
        m_psoDesc.PS.pShaderBytecode = ps.GetCode();
        m_psoDesc.PS.BytecodeLength = ps.GetSize();
    }

    void Pipeline::Finalize(Device& device)
    {
        if( m_psoDesc.InputLayout.pInputElementDescs == nullptr ||
            m_psoDesc.pRootSignature == nullptr ||
            m_psoDesc.VS.pShaderBytecode == nullptr ||
            m_psoDesc.PS.pShaderBytecode == nullptr )
        {
            throw std::runtime_error("Cannot finalize Pipeline!");
        }

        ThrowIfFailed(device.GetDevice()->CreateGraphicsPipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pPipelineState)));
    }
}