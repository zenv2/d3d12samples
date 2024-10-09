#include <iostream>

#include <sample.h>
#include <buffer.h>
#include <shader.h>

#include <fx_vertex.h>
#include <fx_pixel.h>

using namespace std;
using namespace render;

namespace application
{
    class DepthTest : public Sample
    {
        public:

        DepthTest(const uint32_t width, const uint32_t height, const std::string title) :
            Sample(width, height, title, true)
        {
        }

        ~DepthTest()
        {
        }

        void Setup()
        {
            m_pVS = new VertexShader(vertex, strlen(vertex), "VSMain");
            m_pPS = new PixelShader(pixel, strlen(pixel), "PSMain");

            D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            Vertex vertices[] =
            {
                // first quad (closer to camera, blue)
                {{ -0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f }},
                {{  0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f }},
                {{ -0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f }},
                {{  0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f }},

                // second quad (further from camera, green)
                {{ -0.75f,  0.75f,  0.7f}, {0.0f, 1.0f, 0.0f, 1.0f }},
                {{   0.0f,  0.0f, 0.7f}, {0.0f, 1.0f, 0.0f, 1.0f }},
                {{ -0.75f,  0.0f, 0.7f}, {0.0f, 1.0f, 0.0f, 1.0f }},
                {{   0.0f,  0.75f,  0.7f}, {0.0f, 1.0f, 0.0f, 1.0f }}
            };

            uint32_t indices[] =
            {
                // first quad (blue)
                0, 1, 2, // first triangle
                0, 3, 1, // second triangle
            };

            const uint32_t vbSize = sizeof(vertices);
            const uint32_t ibSize = sizeof(indices);

            m_pVB = new VertexBuffer(*m_pDevice, vertices, sizeof(Vertex), vbSize);
            m_pIB = new IndexBuffer(*m_pDevice, indices, ibSize);

            WaitForPreviousFrame();

            m_pPipeline->SetVS(*m_pVS);
            m_pPipeline->SetPS(*m_pPS);
            m_pPipeline->SetInputLayout(inputElementDescs, _countof(inputElementDescs));
            m_pPipeline->SetDescriptors(*m_pDescriptor);
            m_pPipeline->Finalize(*m_pDevice);
        }

        void Run()
        {
            m_pWindow->Show();
            m_pWindow->MainLoop();

            OnDestroy();
        }

        void OnRender()
        {
            const float clearColor[] = {0.2f, 0.0f, 0.2f, 1.0f};
            auto pCommandList = m_pCommander->GetCommandList();

            m_pCommander->Reset(m_pPipeline);

            pCommandList->SetGraphicsRootSignature(m_pDescriptor->GetRootSignature());

            TransitionPresentToRenderTarget();

            pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            pCommandList->IASetVertexBuffers(0, 1, m_pVB->GetView());
            pCommandList->IASetIndexBuffer(m_pIB->GetView());

            pCommandList->RSSetViewports(1, &m_viewport);
            pCommandList->RSSetScissorRects(1, &m_scissorRect);

            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pSwapchain->GetRtvHandle());
            D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pSwapchain->GetDsvHandle());

            pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

            pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
            pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

            pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
            pCommandList->DrawIndexedInstanced(6, 1, 0, 4, 0);

            TransitionRenderTargetToPrsent();

            pCommandList->Close();
            
            m_pCommander->Execute();

            m_pSwapchain->Present();

            WaitForPreviousFrame();
        }

        void OnDestroy()
        {
            WaitForPreviousFrame();
        }

        private:

        static constexpr DXGI_FORMAT rtFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        static constexpr DXGI_FORMAT dsFormat = DXGI_FORMAT_D32_FLOAT;

        VertexShader* m_pVS = nullptr;
        PixelShader* m_pPS = nullptr;
        VertexBuffer* m_pVB = nullptr;
        IndexBuffer* m_pIB = nullptr;
    };
}

int main(int argc, char* argv[])
{
    try
    {
        application::DepthTest app{800, 600, "Depth Test Sample"};

        app.Setup();
        app.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}