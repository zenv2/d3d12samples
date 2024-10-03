#pragma once

namespace render
{
    class Sample
    {
        public:

        Sample() {};
        ~Sample() {};

        virtual void Render() = 0;
    };
}