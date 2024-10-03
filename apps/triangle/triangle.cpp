#include <string.h>

#include <d3d12.h>

#include <triangle.h>

#include <fx_vertex.h>
#include <fx_pixel.h>

namespace application
{
    Triangle::Triangle()
    {

    }

    Triangle::~Triangle()
    {
        if(m_pPS) delete m_pPS;
        if(m_pVS) delete m_pVS;
        if(m_pDescriptor) delete m_pDescriptor;
        if(m_pPipeline) delete m_pPipeline;
    }

    void Triangle::Setup()
    {
        m_pVS = new VertexShader(vertex, strlen(vertex), "VSMain");
        m_pPS = new PixelShader(pixel, strlen(pixel), "PSMain");

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        m_pDescriptor = new Descriptor(m_device);
        m_pPipeline = new Pipeline();

        m_pPipeline->SetInputLayout(inputElementDescs, 2);
        m_pPipeline->SetDescriptors(*m_pDescriptor);
        m_pPipeline->SetVS(*m_pVS);
        m_pPipeline->SetPS(*m_pPS);
        m_pPipeline->Finalize(m_device);
    }

    void Triangle::Run()
    {
        m_window.Show();
        m_window.MainLoop();
    }

    void Triangle::WaitForPreviousFrame()
    {
        m_commander.WaitForPreviousFrame();
        m_swapchain.UpdateFrameIndex();
    }

    void Triangle::Render()
    {
        auto pCommandList = m_commander.GetCommandList();
        m_commander.Reset(m_pPipeline);

        D3D12_RESOURCE_BARRIER rtBarrier;
        m_swapchain.TransitionBarrierPresentToRenderTarget(rtBarrier);
        pCommandList->ResourceBarrier(1, &rtBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_swapchain.GetRtvHandle());

        pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        D3D12_RESOURCE_BARRIER presentBarrier;
        m_swapchain.TransitionBarrierRenderTargetToPresent(presentBarrier);
        pCommandList->ResourceBarrier(1, &presentBarrier);

        pCommandList->Close();

        m_commander.Execute();

        m_swapchain.Present();

        WaitForPreviousFrame();
    }
}