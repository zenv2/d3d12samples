#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <windows.h>
#include <stdint.h>

namespace render
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if(FAILED(hr))
        {
            //Todo: Change to runtime_error() with HR to string
            throw std::exception();
        }
    }

    //Todo: Templetize with _FILE_ and _LINE_ from caller if possible
    inline void ThrowIfAssert(bool condition)
    {
        if(!condition)
        {
            throw std::exception();
        }
    }

    inline void SafeRelease(IUnknown* ptr)
    {
        if(ptr != nullptr)
        {
            ptr->Release();
        }
    }

#if defined(_DEBUG)
    #define LOG(x) { std::cout << __FUNCTION__ << "() : " << x << std::endl; }
#else
    #define LOG(x) 
#endif
}