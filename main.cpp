#include "Game.h"
#include "Graphics.h"
#include <iostream>

int main() {
    GameGUI gui;
    if (gui.initialize()) {
        gui.run();
    } else {
        std::cerr << "Не удалось запустить графический интерфейс." << std::endl;
        return 1;
    }
    return 0;
}