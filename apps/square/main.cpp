#include <iostream>
#include <square.h>

using namespace std;
using namespace application;

int main(int argc, char* argv[])
{
    try
    {
        Square square{900, 900, "DX12 Square"};

        square.Setup();
        square.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << typeid(e).name() << '\n';
    }
}