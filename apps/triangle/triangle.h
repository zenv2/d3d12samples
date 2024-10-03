#include <window.h>
#include <sample.h>
#include <device.h>
#include <commander.h>
#include <swapchain.h>
#include <descriptor.h>
#include <pipeline.h>
#include <shader.h>

using namespace render;

namespace application
{
    class Triangle : public Sample
    {
        public:
        
        Triangle();
        ~Triangle();

        void Setup();
        void Run();

        void Render();
    
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
    private:
        Window m_window{this, WIDTH, HEIGHT, "DX12 Triangle"};
        Device m_device{};
        Commander m_commander{m_device};
        Swapchain m_swapchain{m_window, m_device, m_commander};

        Descriptor* m_pDescriptor = nullptr;
        Pipeline* m_pPipeline = nullptr;
        VertexShader* m_pVS = nullptr;
        PixelShader* m_pPS = nullptr;

        void WaitForPreviousFrame();
    };
}