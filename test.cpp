#include <iostream>

int main(int argc, char** argv)
{

    int x = std::stoi(argv[1]);
    int y = std::stoi(argv[2]);
    
    if (x > 0){
        x = -1*x;
    }

    if (y > 0){
        y = -1*y;
    }

    std::cout << x + y;
    return 0;
}