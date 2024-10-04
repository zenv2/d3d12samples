#include <string.h>

#include <d3d12.h>

#include <triangle.h>

#include <fx_vertex.h>
#include <fx_pixel.h>

namespace application
{
    Triangle::Triangle() :
        m_viewport({0.0f, 0.0f, static_cast<float>(WIDTH), static_cast<float>(HEIGHT), .1f, 1000.f}),
        m_scissorRect({0, 0, static_cast<LONG>(WIDTH), static_cast<LONG>(HEIGHT)})
    {
    }

    Triangle::~Triangle()
    {
        if(m_pPS) delete m_pPS;
        if(m_pVS) delete m_pVS;
        if(m_pVB) delete m_pVB;
        if(m_pDescriptor) delete m_pDescriptor;
        if(m_pPipeline) delete m_pPipeline;
    }

    void Triangle::Setup()
    {
        m_pVS = new VertexShader(vertex, strlen(vertex), "VSMain");
        m_pPS = new PixelShader(pixel, strlen(pixel), "PSMain");

        Vertex vertices[] =
        {
            { { 0.0f, 0.25f * m_aspectRatio, 0.0 }, {1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * m_aspectRatio, 0.0f }, {0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * m_aspectRatio, 0.0f }, {0.0f, 0.0f, 1.0f, 1.0f } }
        };

        const uint32_t vertexBufferSize = sizeof(vertices);

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        uint32_t indices[] = {0, 1, 2};
        const uint32_t indexBufferSize = sizeof(indices);

        m_pDescriptor = new Descriptor(m_device);
        m_pPipeline = new Pipeline();

        m_pVB = new VertexBuffer(m_device, &vertices, sizeof(Vertex), vertexBufferSize);
        m_pIB = new IndexBuffer(m_device, &indices, indexBufferSize);

        // Wait until buffers are uploaded to GPU
        WaitForPreviousFrame();

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

        OnDestroy();
    }

    void Triangle::WaitForPreviousFrame()
    {
        m_commander.WaitForPreviousFrame();
        m_swapchain.UpdateFrameIndex();
    }

    void Triangle::OnRender()
    {
        auto pCommandList = m_commander.GetCommandList();
        m_commander.Reset(m_pPipeline);

        pCommandList->SetGraphicsRootSignature(m_pDescriptor->GetRootSignature());

        pCommandList->RSSetViewports(1, &m_viewport);
        pCommandList->RSSetScissorRects(1, &m_scissorRect);

        D3D12_RESOURCE_BARRIER rtBarrier;
        m_swapchain.TransitionBarrierPresentToRenderTarget(rtBarrier);
        pCommandList->ResourceBarrier(1, &rtBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_swapchain.GetRtvHandle());

        pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pCommandList->IASetVertexBuffers(0, 1, m_pVB->GetView());
        pCommandList->IASetIndexBuffer(m_pIB->GetView());

        pCommandList->DrawInstanced(3, 1, 0, 0);

        D3D12_RESOURCE_BARRIER presentBarrier;
        m_swapchain.TransitionBarrierRenderTargetToPresent(presentBarrier);
        pCommandList->ResourceBarrier(1, &presentBarrier);

        pCommandList->Close();

        m_commander.Execute();

        m_swapchain.Present();

        WaitForPreviousFrame();
    }

    void Triangle::OnDestroy()
    {
        WaitForPreviousFrame();
    }
}