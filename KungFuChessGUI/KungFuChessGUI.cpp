// KungFuChessGUI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "img.hpp"
#include <iostream>
#include <vector>     
#include <string>

int main()
{
    try {
        std::cout << "Testing Img class..." << std::endl;

        Img img;
        //img.read(R"(C:\Users\user1\Documents\מלכי\kamateck\KungFuChess\KungFuChessGUI\pictures\board_classic.png)", { 640, 480 }, true);
        img.read(R"(.\pictures\board_classic.png)", { 640, 480 }, true);
        img.put_text("Hello, Img!", 150, 360, 1.0, { 0,0,0 });
        img.show();

        std::cout << "Img class test completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
