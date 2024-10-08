#include <iostream>
#include <triangle.h>

using namespace std;

static constexpr uint32_t WIDTH = 800;
static constexpr uint32_t HEIGHT = 600;

int main(int argc, char* argv[])
{
    cout << "Hello World" << endl;

    try
    {
        application::Triangle triangle{WIDTH, HEIGHT, "Triangle App"};

        triangle.Setup();
        triangle.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << typeid(e).name() << '\n';
    }
}