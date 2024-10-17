#include <iostream>
#include <windows.h>
#include <sample.h>
#include <buffer.h>
#include <shader.h>

#include <fx_vertex.h>
#include <fx_pixel.h>

using namespace std;
using namespace render;

namespace application
{
    struct Constants
    {
        DirectX::XMFLOAT4 colorMultiplier;
    };

    class ConstantsQuad : public Sample
    {
        public:

        ConstantsQuad(const uint32_t width, const uint32_t height, const std::string title) :
            Sample(width, height, title, true)
        {
            ZeroMemory(&m_cbColorInfo, sizeof(m_cbColorInfo));
            m_cbColorInfo.colorMultiplier.x = 0.0f;
            m_cbColorInfo.colorMultiplier.y = 0.0f;
            m_cbColorInfo.colorMultiplier.z = 0.0f;
            m_cbColorInfo.colorMultiplier.w = 1.0f;
        }

        ~ConstantsQuad()
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
                {{ -0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f }},
                {{  0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f }},
                {{ -0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f }},
                {{  0.5f,  0.5f, 0.0f}, {0.5f, 0.0f, 0.5f, 1.0f }},
            };

            uint32_t indices[] =
            {
                0, 1, 2, // first triangle
                0, 3, 1, // second triangle
            };

            const uint32_t vbSize = sizeof(vertices);
            const uint32_t ibSize = sizeof(indices);

            m_pVB = new VertexBuffer(*m_pDevice, vertices, sizeof(Vertex), vbSize);
            m_pIB = new IndexBuffer(*m_pDevice, indices, ibSize);

            WaitForPreviousFrame();

            SetupDescriptor();

            m_pPipeline->SetVS(*m_pVS);
            m_pPipeline->SetPS(*m_pPS);
            m_pPipeline->SetInputLayout(inputElementDescs, _countof(inputElementDescs));
            m_pPipeline->SetDescriptors(*m_pDescriptor);
            m_pPipeline->Finalize(*m_pDevice);
        }

        void SetupDescriptor()
        {
            D3D12_ROOT_PARAMETER rootParam = {};
            rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            rootParam.Constants.ShaderRegister = 3;
            rootParam.Constants.RegisterSpace = 0;
            rootParam.Constants.Num32BitValues = 4;
            rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

            D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
            rsDesc.NumParameters = 1;
            rsDesc.pParameters = &rootParam;
            rsDesc.NumStaticSamplers = 0;
            rsDesc.pStaticSamplers = nullptr;
            rsDesc.Flags = 
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | 
                D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
            
            m_pDescriptor->CreateRootSignature(&rsDesc);
        }

        void Run()
        {
            m_pWindow->Show();
            m_pWindow->MainLoop();

            OnDestroy();
        }

        void OnUpdate()
        {
            static float rIncrement = 0.01f;
            static float gIncrement = 0.04f;
            static float bIncrement = 0.07f;

            m_cbColorInfo.colorMultiplier.x += rIncrement;
            m_cbColorInfo.colorMultiplier.y += gIncrement;
            m_cbColorInfo.colorMultiplier.z += bIncrement;

            if(m_cbColorInfo.colorMultiplier.x >= 1.0f ||  m_cbColorInfo.colorMultiplier.x <= 0.0f)
            {
                m_cbColorInfo.colorMultiplier.x =  m_cbColorInfo.colorMultiplier.x >= 1.0f ? 1.0f : 0.0f;
                rIncrement = -rIncrement;
            }

            if(m_cbColorInfo.colorMultiplier.y >= 1.0f ||  m_cbColorInfo.colorMultiplier.y <= 0.0f)
            {
                m_cbColorInfo.colorMultiplier.y =  m_cbColorInfo.colorMultiplier.y >= 1.0f ? 1.0f : 0.0f;
                gIncrement = -gIncrement;
            }

            if(m_cbColorInfo.colorMultiplier.z >= 1.0f ||  m_cbColorInfo.colorMultiplier.z <= 0.0f)
            {
                m_cbColorInfo.colorMultiplier.z =  m_cbColorInfo.colorMultiplier.z >= 1.0f ? 1.0f : 0.0f;
                bIncrement = -bIncrement;
            }
        }

        void OnRender()
        {
            Sleep(50);
            const float clearColor[] = {0.8f, 0.8f, 0.8f, 1.0f};
            auto pCommandList = m_pCommander->GetCommandList();

            m_pCommander->Reset(m_pPipeline);

            // Set root signature first,
            // then set the root constants for the corresponding root parameter index.
            pCommandList->SetGraphicsRootSignature(m_pDescriptor->GetRootSignature());
            pCommandList->SetGraphicsRoot32BitConstants(0,4, &m_cbColorInfo, 0);

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

        Constants m_cbColorInfo;

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
        application::ConstantsQuad app{800, 600, "Root Constants"};

        app.Setup();
        app.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}