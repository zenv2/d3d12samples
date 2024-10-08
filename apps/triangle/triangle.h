#include <window.h>
#include <sample.h>
#include <shader.h>
#include <buffer.h>

using namespace render;

namespace application
{
    class Triangle : public Sample
    {
        public:
        
        Triangle(const uint32_t width, const uint32_t height, const std::string title);
        ~Triangle();

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