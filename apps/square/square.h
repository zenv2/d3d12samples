#include <window.h>
#include <sample.h>
#include <device.h>
#include <commander.h>
#include <swapchain.h>
#include <descriptor.h>
#include <pipeline.h>
#include <shader.h>
#include <buffer.h>

using namespace render;

namespace application
{
    class Square : public Sample
    {
        public:
        
        Square(const uint32_t width, const uint32_t height, const std::string title);
        ~Square();

        void Setup();
        void Run();

        void OnRender();
        void OnDestroy();
    
    private:

        VertexShader* m_pVS = nullptr;
        PixelShader* m_pPS = nullptr;
        VertexBuffer* m_pVB = nullptr;
        IndexBuffer* m_pIB = nullptr;

    };
}