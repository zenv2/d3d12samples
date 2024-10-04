#pragma once

#include <directxmath.h>

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
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        float m_aspectRatio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

        Sample() {};
        ~Sample() {};

        virtual void OnRender() = 0;
    };
}