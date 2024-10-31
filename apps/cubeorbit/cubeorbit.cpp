#include <iostream>
#include <windows.h>
#include <sample.h>
#include <buffer.h>
#include <shader.h>

#include <fx_vertex.h>
#include <fx_pixel.h>

using namespace std;
using namespace render;
using namespace DirectX;

namespace application
{
    struct ConstantsPerObject
    {
        XMFLOAT4X4 m_wvpMat;
    };

    class CubeOrbit : public Sample
    {
        public:

        CubeOrbit(const uint32_t width, const uint32_t height, const std::string title) :
            Sample(width, height, title, true)
        {
            m_frameCount = m_pSwapchain->GetFrameCount();
            m_ppConstantBuffers = new ConstantBuffer*[m_frameCount];
        }

        ~CubeOrbit()
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
                // front face
                frontTopLeft,
                frontBottomRight,
                frontBottomLeft,
                frontTopRight,

                // right face
                frontBottomRight,
                backTopRight,
                backBottomRight,
                frontTopRight,
                
                // left face
                backTopLeft,
                frontBottomLeft,
                backBottomLeft,
                frontTopLeft,

                // back face
                backTopRight,
                backBottomLeft,
                backBottomRight,
                backTopLeft,

                // top face
                frontTopLeft,
                backTopRight,
                frontTopRight,
                backTopLeft,

                // bottom face
                backBottomRight,
                frontBottomLeft,
                frontBottomRight,
                backBottomLeft
            };

            uint32_t indices[] =
            {
                // front face
                0, 1, 2, // first triangle
                0, 3, 1, // second triangle

                // left face
                4, 5, 6, // first triangle
                4, 7, 5, // second triangle

                // right face
                8, 9, 10, // first triangle
                8, 11, 9, // second triangle

                // back face
                12, 13, 14, // first triangle
                12, 15, 13, // second triangle

                // top face
                16, 17, 18, // first triangle
                16, 19, 17, // second triangle

                // bottom face
                20, 21, 22, // first triangle
                20, 23, 21, // second triangle
            };

            const uint32_t vbSize = sizeof(vertices);
            const uint32_t ibSize = sizeof(indices);

            m_numCubeIndices = sizeof(indices)/sizeof(indices[0]);

            m_pVB = new VertexBuffer(*m_pDevice, vertices, sizeof(Vertex), vbSize);
            m_pIB = new IndexBuffer(*m_pDevice, indices, ibSize);

            ZeroMemory(&m_cbPerObj, sizeof(ConstantsPerObject));

            for(uint32_t i =0; i < m_frameCount; i++)
            {
                m_ppConstantBuffers[i] = new ConstantBuffer(*m_pDevice, sizeof(ConstantsPerObject), m_numObjects);
                for(uint32_t j=0; j < m_numObjects; j++ )
                {
                    m_ppConstantBuffers[i]->Upload(&m_cbPerObj, j);
                }
            }

            WaitForPreviousFrame();

            SetupDescriptor();

            m_pPipeline->SetVS(*m_pVS);
            m_pPipeline->SetPS(*m_pPS);
            m_pPipeline->SetInputLayout(inputElementDescs, _countof(inputElementDescs));
            m_pPipeline->SetDescriptors(*m_pDescriptor);
            m_pPipeline->Finalize(*m_pDevice);

            XMMATRIX mat = XMMatrixPerspectiveFovLH(45.0f * (3.14f/180.0f), m_aspectRatio, 0.1f, 1000.0f);
            XMStoreFloat4x4(&m_cameraProjMat, mat);

            m_cameraPosition = XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f);
            m_cameraTarget = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
            m_cameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

            XMVECTOR cPos = XMLoadFloat4(&m_cameraPosition);
            XMVECTOR cTar = XMLoadFloat4(&m_cameraTarget);
            XMVECTOR cUp = XMLoadFloat4(&m_cameraUp);

            mat = XMMatrixLookAtLH(cPos, cTar, cUp);

            XMStoreFloat4x4(&m_cameraViewMat, mat);

            m_cube1Pos = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
            XMVECTOR posVec = XMLoadFloat4(&m_cube1Pos);

            mat = XMMatrixTranslationFromVector(posVec);
            XMStoreFloat4x4(&m_cube1RotMat, XMMatrixIdentity());
            XMStoreFloat4x4(&m_cube1WorldMat, mat);

            m_cube2PosOffset = XMFLOAT4(1.5f, 0.0f, 0.0f, 0.0f);
            posVec = XMLoadFloat4(&m_cube1Pos) + XMLoadFloat4(&m_cube2PosOffset);

            mat = XMMatrixTranslationFromVector(posVec);
            XMStoreFloat4x4(&m_cube2RotMat, XMMatrixIdentity());
            XMStoreFloat4x4(&m_cube2WorldMat, mat);

        }

        void SetupDescriptor()
        {
            D3D12_ROOT_PARAMETER rootParam = {};
            rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            rootParam.Descriptor.ShaderRegister = 0;
            rootParam.Descriptor.RegisterSpace = 0;
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
            uint32_t frameIndex = m_pSwapchain->GetFrameIndex();
            XMMATRIX rotXMat = XMMatrixRotationX(0.01f);
            XMMATRIX rotYMat = XMMatrixRotationY(0.02f);
            XMMATRIX rotZMat = XMMatrixRotationZ(0.03f);

            XMMATRIX rotMat = XMLoadFloat4x4(&m_cube1RotMat) * rotXMat * rotYMat * rotZMat;
            XMStoreFloat4x4(&m_cube1RotMat, rotMat);

            XMMATRIX translationMat = XMMatrixTranslationFromVector(XMLoadFloat4(&m_cube1Pos));

            XMMATRIX worldMat = rotMat * translationMat;
            XMStoreFloat4x4(&m_cube1WorldMat, worldMat);

            XMMATRIX viewMat = XMLoadFloat4x4(&m_cameraViewMat);
            XMMATRIX projMat = XMLoadFloat4x4(&m_cameraProjMat);
            XMMATRIX wvpMat = XMLoadFloat4x4(&m_cube1WorldMat) * viewMat * projMat;
            XMMATRIX transposed = XMMatrixTranspose(wvpMat);
            XMStoreFloat4x4(&m_cbPerObj.m_wvpMat, transposed);

            m_ppConstantBuffers[frameIndex]->Upload(&m_cbPerObj, 0);

            rotXMat = XMMatrixRotationX(0.03f);
            rotYMat = XMMatrixRotationY(0.02f);
            rotZMat = XMMatrixRotationZ(0.01f);

            rotMat = rotZMat * (XMLoadFloat4x4(&m_cube2RotMat) * (rotXMat * rotYMat));
            XMStoreFloat4x4(&m_cube2RotMat, rotMat);

            XMMATRIX translationOffsetMatrix = XMMatrixTranslationFromVector(XMLoadFloat4(&m_cube2PosOffset));

            XMMATRIX scaleMat = XMMatrixScaling(0.5f, 0.5f, 0.5f);

            worldMat = scaleMat * translationOffsetMatrix * rotMat * translationMat;

            wvpMat = XMLoadFloat4x4(&m_cube2WorldMat) * viewMat * projMat;
            transposed = XMMatrixTranspose(wvpMat);
            XMStoreFloat4x4(&m_cbPerObj.m_wvpMat, transposed);

            m_ppConstantBuffers[frameIndex]->Upload(&m_cbPerObj, 1);

            XMStoreFloat4x4(&m_cube2WorldMat, worldMat);
        }

        void OnRender()
        {
            uint32_t frameIndex = m_pSwapchain->GetFrameIndex();
            const float clearColor[] = {0.3f, 0.3f, 0.3f, 1.0f};
            auto pCommandList = m_pCommander->GetCommandList();

            m_pCommander->Reset(m_pPipeline);

            // Set root signature first,
            // then set the root constants for the corresponding root parameter index.
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

            pCommandList->SetGraphicsRootConstantBufferView(0, m_ppConstantBuffers[frameIndex]->GetGpuVa(0));
            pCommandList->DrawIndexedInstanced(m_numCubeIndices, 1, 0, 0, 0);

            pCommandList->SetGraphicsRootConstantBufferView(0, m_ppConstantBuffers[frameIndex]->GetGpuVa(1));
            pCommandList->DrawIndexedInstanced(m_numCubeIndices, 1, 0, 0, 0);

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

        static constexpr Vertex frontTopLeft = { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} };
        static constexpr Vertex frontTopRight = { {0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} };
        static constexpr Vertex frontBottomLeft = { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} };
        static constexpr Vertex frontBottomRight = { {0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f} };

        static constexpr Vertex backTopLeft = { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} };
        static constexpr Vertex backTopRight = { {0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} };
        static constexpr Vertex backBottomLeft = { {-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f} };
        static constexpr Vertex backBottomRight = { {0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} };

        uint32_t m_frameCount;
        const uint32_t m_numObjects = 2;
        uint32_t m_numCubeIndices;

        ConstantsPerObject m_cbPerObj;

        VertexShader* m_pVS = nullptr;
        PixelShader* m_pPS = nullptr;
        VertexBuffer* m_pVB = nullptr;
        IndexBuffer* m_pIB = nullptr;
        ConstantBuffer** m_ppConstantBuffers;

        XMFLOAT4X4 m_cameraProjMat;
        XMFLOAT4X4 m_cameraViewMat;

        XMFLOAT4 m_cameraPosition;
        XMFLOAT4 m_cameraTarget;
        XMFLOAT4 m_cameraUp;

        XMFLOAT4X4 m_cube1WorldMat;
        XMFLOAT4X4 m_cube1RotMat;
        XMFLOAT4 m_cube1Pos;

        XMFLOAT4X4 m_cube2WorldMat;
        XMFLOAT4X4 m_cube2RotMat;
        XMFLOAT4 m_cube2PosOffset;
    };
}

int main(int argc, char* argv[])
{
    try
    {
        application::CubeOrbit app{800, 600, "Cube Orbit"};

        app.Setup();
        app.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}