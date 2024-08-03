#include <iostream>
#include <triangle.h>

using namespace std;

int main(int argc, char* argv[])
{
    cout << "Hello World" << endl;

    try
    {
        application::Triangle triangle{};

        triangle.Setup();
        triangle.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << typeid(e).name() << '\n';
    }
}