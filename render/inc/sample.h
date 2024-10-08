#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <window.h>
#include <device.h>
#include <commander.h>
#include <swapchain.h>
#include <descriptor.h>
#include <pipeline.h>

using namespace std;

namespace render
{
    typedef struct
    {
        float position[3];
        float color[4];
    } Vertex;

    class Sample
    {
        public:
        uint32_t m_width = 800;
        uint32_t m_height = 600;
        float m_aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);

        Sample();
        Sample(uint32_t width, uint32_t height, std::string title);
        ~Sample();

        virtual void Setup() = 0;
        virtual void Run() = 0;
        virtual void OnRender() = 0;
        virtual void OnDestroy() = 0;

        D3D12_VIEWPORT m_viewport;
        D3D12_RECT m_scissorRect;

        protected:
        unique_ptr<Window> m_pWindow = nullptr;
        unique_ptr<Device> m_pDevice = nullptr;
        unique_ptr<Commander> m_pCommander = nullptr;
        unique_ptr<Swapchain> m_pSwapchain = nullptr;

        Descriptor* m_pDescriptor = nullptr;
        Pipeline* m_pPipeline = nullptr;

        void WaitForPreviousFrame();

        private:
        std::string m_title;
    };
}