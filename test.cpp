#include <iostream>

int main(int argc, char** argv)
{
    double x = std::stod(argv[1]);
    double y = std::stod(argv[2]);
    
    std::cout << -1*(x*x + y*y);
    return 0;
}