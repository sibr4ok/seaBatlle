#include "Player.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cctype>

// AbstractPlayer

AbstractPlayer::AbstractPlayer() : name("Player"), shotsCount(0), hitsCount(0) {}

AbstractPlayer::AbstractPlayer(const std::string& name) 
    : name(name), shotsCount(0), hitsCount(0) {}

double AbstractPlayer::getAccuracy() const {
    if (shotsCount == 0) return 0.0;
    return static_cast<double>(hitsCount) / shotsCount * 100.0;
}

// HumanPlayer

HumanPlayer::HumanPlayer() : AbstractPlayer("Игрок") {}

HumanPlayer::HumanPlayer(const std::string& name) : AbstractPlayer(name) {}

std::pair<int, int> HumanPlayer::makeMove() {
    std::string input;
    std::pair<int, int> coords;
    
    while (true) {
        std::cout << "Введите координаты (например, A5 или Б3): ";
        std::cin >> input;
        
        coords = parseInput(input);
        
        if (coords. first >= 0 && coords.first < 10 && 
            coords.second >= 0 && coords. second < 10) {
            return coords;
        }
        
        std::cout << "Неверный формат!  Используйте букву (A-J или А-К) и цифру (1-10).\n";
    }
}

std::pair<int, int> HumanPlayer::parseInput(const std::string& input) const {
    if (input.empty()) return {-1, -1};
    
    int x = -1;
    int y = -1;
    
    // парсим букву (латиница или кириллица)
    char letter = std::toupper(input[0]);
    
    // латинские буквы A-J
    if (letter >= 'A' && letter <= 'J') {
        x = letter - 'A';
    }
    // русские буквы (UTF-8)
    else {
        // простая обработка русских букв
        std::string rusLetters = "АБВГДЕЖЗИК";
        size_t pos = rusLetters.find(input. substr(0, 2));
        if (pos != std::string::npos) {
            x = pos / 2;
        }
    }
    
    // парсим число
    size_t numStart = 1;
    // для UTF-8 русских букв пропускаем 2 байта
    if (input.length() > 1 && (unsigned char)input[0] > 127) {
        numStart = 2;
    }
    
    if (numStart < input.length()) {
        try {
            y = std::stoi(input. substr(numStart)) - 1;
        } catch (... ) {
            y = -1;
        }
    }
    
    return {x, y};
}

void HumanPlayer::placeShips() {
    std::cout << "\n=== Расстановка кораблей ===\n";
    std::cout << "Выберите способ:\n";
    std::cout << "1. Автоматическая расстановка\n";
    std::cout << "2.  Ручная расстановка\n";
    std::cout << "Ваш выбор: ";
    
    int choice;
    std::cin >> choice;
    
    if (choice == 1) {
        field.placeAllShipsAuto();
        std::cout << "\nКорабли расставлены автоматически!\n";
        field.drawWithColors(false);
    } else {
        // ручная расстановка
        struct ShipInfo {
            int size;
            int count;
            std::string name;
        };
        
        std::vector<ShipInfo> shipsToPlace = {
            {4, 1, "Линкор (4 палубы)"},
            {3, 2, "Крейсер (3 палубы)"},
            {2, 3, "Эсминец (2 палубы)"},
            {1, 4, "Катер (1 палуба)"}
        };
        
        for (const auto& shipInfo : shipsToPlace) {
            for (int i = 0; i < shipInfo.count; i++) {
                bool placed = false;
                while (!placed) {
                    field.drawWithColors(false);
                    std::cout << "\nРазмещение: " << shipInfo.name 
                              << " (" << (i + 1) << "/" << shipInfo.count << ")\n";
                    
                    std::cout << "Введите координаты (например, A5): ";
                    std::string input;
                    std::cin >> input;
                    
                    auto coords = parseInput(input);
                    
                    bool vertical = true;
                    if (shipInfo.size > 1) {
                        std::cout << "Направление (1-горизонтально, 2-вертикально): ";
                        int dir;
                        std::cin >> dir;
                        vertical = (dir == 2);
                    }
                    
                    if (field.placeShip(coords.first, coords.second, shipInfo.size, vertical)) {
                        placed = true;
                        std::cout << "Корабль размещён!\n";
                    } else {
                        std::cout << "Невозможно разместить корабль здесь!  Попробуйте снова.\n";
                    }
                }
            }
        }
        
        std::cout << "\nВсе корабли расставлены!\n";
        field.drawWithColors(false);
    }
}

// ComputerPlayer

ComputerPlayer::ComputerPlayer() 
    : AbstractPlayer("Компьютер"), lastHitX(-1), lastHitY(-1), isHunting(false) {
    initAvailableCells();
}

ComputerPlayer::ComputerPlayer(const std::string& name) 
    : AbstractPlayer(name), lastHitX(-1), lastHitY(-1), isHunting(false) {
    initAvailableCells();
}

void ComputerPlayer::initAvailableCells() {
    availableCells.clear();
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            availableCells. push_back({x, y});
        }
    }
    
    // перемешиваем для случайного выбора
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(availableCells.begin(), availableCells.end(), gen);
}

void ComputerPlayer::addPriorityCells(int x, int y) {
    // добавляем соседние клетки в приоритет
    std::vector<std::pair<int, int>> neighbors = {
        {x - 1, y}, {x + 1, y}, {x, y - 1}, {x, y + 1}
    };
    
    for (const auto& n : neighbors) {
        if (n. first >= 0 && n.first < 10 && n.second >= 0 && n.second < 10) {
            // проверяем, что клетка ещё доступна
            auto it = std::find(availableCells. begin(), availableCells.end(), n);
            if (it != availableCells.end()) {
                // проверяем, что её нет в приоритетных
                auto pit = std::find(priorityCells. begin(), priorityCells.end(), n);
                if (pit == priorityCells. end()) {
                    priorityCells. push_back(n);
                }
            }
        }
    }
}

std::pair<int, int> ComputerPlayer::makeMove() {
    std::pair<int, int> target;
    
    if (! priorityCells.empty()) {
        // есть приоритетные клетки - добиваем корабль
        target = priorityCells.back();
        priorityCells. pop_back();
    } else if (!availableCells.empty()) {
        // стреляем в случайную клетку
        target = availableCells.back();
        availableCells. pop_back();
    } else {
        target = {0, 0};
    }
    
    // удаляем из доступных
    auto it = std::find(availableCells.begin(), availableCells.end(), target);
    if (it != availableCells.end()) {
        availableCells.erase(it);
    }
    
    return target;
}

void ComputerPlayer::onHit(int x, int y) {
    isHunting = true;
    lastHitX = x;
    lastHitY = y;
    addPriorityCells(x, y);
}

void ComputerPlayer::onDestroyed() {
    isHunting = false;
    priorityCells.clear();
}

void ComputerPlayer::placeShips() {
    field.placeAllShipsAuto();
}

void ComputerPlayer::reset() {
    initAvailableCells();
    priorityCells.clear();
    lastHitX = -1;
    lastHitY = -1;
    isHunting = false;
}