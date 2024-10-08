#include <string.h>

#include <d3d12.h>

#include <square.h>

#include <fx_vertex.h>
#include <fx_pixel.h>

namespace application
{
    Square::Square(const uint32_t width, const uint32_t height, const std::string title) :
        Sample(width, height, title)
    {
    }

    Square::~Square()
    {
        if(m_pPS) delete m_pPS;
        if(m_pVS) delete m_pVS;
        if(m_pVB) delete m_pVB;
    }

    void Square::Setup()
    {
        m_pVS = new VertexShader(vertex, strlen(vertex), "VSMain");
        m_pPS = new PixelShader(pixel, strlen(pixel), "PSMain");

        Vertex vertices[] =
        {
            { { -0.5f, 0.5f , 0.0f }, {1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.5f, -0.5f , 0.0f }, {0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.5f, -0.5f , 0.0f }, {0.0f, 0.0f, 1.0f, 1.0f } },
            { { 0.5f, 0.5f , 0.0f }, {0.5f, 0.5f, 0.0f, 1.0f } }
        };

        const uint32_t vertexBufferSize = sizeof(vertices);

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        uint32_t indices[] = {0, 1, 2, 0, 3, 1};
        const uint32_t indexBufferSize = sizeof(indices);


        m_pVB = new VertexBuffer(*m_pDevice, &vertices, sizeof(Vertex), vertexBufferSize);
        m_pIB = new IndexBuffer(*m_pDevice, &indices, indexBufferSize);

        // Wait until buffers are uploaded to GPU
        WaitForPreviousFrame();

        m_pPipeline->SetInputLayout(inputElementDescs, 2);
        m_pPipeline->SetDescriptors(*m_pDescriptor);
        m_pPipeline->SetVS(*m_pVS);
        m_pPipeline->SetPS(*m_pPS);
        m_pPipeline->Finalize(*m_pDevice);
    }

    void Square::Run()
    {
        m_pWindow->Show();
        m_pWindow->MainLoop();

        OnDestroy();
    }

    void Square::OnRender()
    {
        auto pCommandList = m_pCommander->GetCommandList();
        m_pCommander->Reset(m_pPipeline);

        pCommandList->SetGraphicsRootSignature(m_pDescriptor->GetRootSignature());

        pCommandList->RSSetViewports(1, &m_viewport);
        pCommandList->RSSetScissorRects(1, &m_scissorRect);

        D3D12_RESOURCE_BARRIER rtBarrier;
        m_pSwapchain->TransitionBarrierPresentToRenderTarget(rtBarrier);
        pCommandList->ResourceBarrier(1, &rtBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pSwapchain->GetRtvHandle());

        pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        const float clearColor[] = { 0.1f, 0.2f, 0.4f, 1.0f };
        pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pCommandList->IASetVertexBuffers(0, 1, m_pVB->GetView());
        pCommandList->IASetIndexBuffer(m_pIB->GetView());

        pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

        D3D12_RESOURCE_BARRIER presentBarrier;
        m_pSwapchain->TransitionBarrierRenderTargetToPresent(presentBarrier);
        pCommandList->ResourceBarrier(1, &presentBarrier);

        pCommandList->Close();

        m_pCommander->Execute();

        m_pSwapchain->Present();

        WaitForPreviousFrame();
    }

    void Square::OnDestroy()
    {
        WaitForPreviousFrame();
    }
}