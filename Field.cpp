#include "Field.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <algorithm>

// конструктор по умолчанию
Field::Field() : size(DEFAULT_FIELD_SIZE), destroyedShipsCount(0) {
    cells.resize(size, std::vector<Cell>(size));
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            cells[y][x] = Cell(x, y);
        }
    }
}

// конструктор с размером
Field::Field(int size) : size(size), destroyedShipsCount(0) {
    cells.resize(size, std::vector<Cell>(size));
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            cells[y][x] = Cell(x, y);
        }
    }
}

// конструктор копирования
Field::Field(const Field& other) 
    : size(other.size), cells(other.cells), destroyedShipsCount(other.destroyedShipsCount) {
    for (const auto& ship : other.ships) {
        ships. push_back(std::make_unique<Ship>(*ship));
    }
}

// оператор присваивания
Field& Field::operator=(const Field& other) {
    if (this != &other) {
        size = other.size;
        cells = other.cells;
        destroyedShipsCount = other. destroyedShipsCount;
        ships. clear();
        for (const auto& ship : other.ships) {
            ships. push_back(std::make_unique<Ship>(*ship));
        }
    }
    return *this;
}

bool Field::isValidPosition(int x, int y) const {
    return x >= 0 && x < size && y >= 0 && y < size;
}

bool Field::canPlaceShip(int x, int y, int shipSize, bool vertical) const {
    // проверяем, что корабль помещается на поле
    if (vertical) {
        if (y + shipSize > size) return false;
    } else {
        if (x + shipSize > size) return false;
    }
    
    // проверяем, что все клетки свободны
    for (int i = 0; i < shipSize; i++) {
        int checkX = vertical ? x : x + i;
        int checkY = vertical ? y + i : y;
        
        if (! isValidPosition(checkX, checkY)) return false;
        
        const Cell& cell = cells[checkY][checkX];
        if (cell.getState() != CellState::Empty && cell.getState() != CellState::Blocked) {
            return false;
        }
        if (cell.getState() == CellState::Blocked) {
            return false;
        }
    }
    
    return true;
}

void Field::markSurroundingCells(const Ship& ship) {
    auto coords = ship.getDecksCoordinates();
    
    for (const auto& coord : coords) {
        // проверяем все 8 соседних клеток
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = coord.first + dx;
                int ny = coord.second + dy;
                
                if (isValidPosition(nx, ny)) {
                    Cell& cell = cells[ny][nx];
                    if (cell.getState() == CellState::Empty) {
                        cell.setState(CellState::Blocked);
                    }
                }
            }
        }
    }
}

void Field::markDestroyedShipCells(const Ship& ship) {
    auto coords = ship.getDecksCoordinates();
    
    // помечаем палубы как уничтоженные
    for (const auto& coord : coords) {
        cells[coord.second][coord.first].setState(CellState::Destroyed);
    }
    
    // помечаем окружающие клетки как промахи (для отображения)
    for (const auto& coord : coords) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = coord. first + dx;
                int ny = coord.second + dy;
                
                if (isValidPosition(nx, ny)) {
                    Cell& cell = cells[ny][nx];
                    if (cell. getState() == CellState::Blocked || 
                        cell.getState() == CellState::Empty) {
                        cell.setState(CellState::Miss);
                    }
                }
            }
        }
    }
}

bool Field::placeShip(int x, int y, int shipSize, bool vertical) {
    if (! canPlaceShip(x, y, shipSize, vertical)) {
        return false;
    }
    
    auto ship = std::make_unique<Ship>(shipSize, x, y, vertical);
    
    // помечаем клетки как занятые кораблём
    auto coords = ship->getDecksCoordinates();
    for (const auto& coord : coords) {
        cells[coord.second][coord. first].setState(CellState::Ship);
        cells[coord.second][coord. first].setShipId(ship->getId());
    }
    
    // помечаем окружающие клетки
    markSurroundingCells(*ship);
    
    ships.push_back(std::move(ship));
    return true;
}

bool Field::placeShipAuto(int shipSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distPos(0, size - 1);
    std::uniform_int_distribution<> distDir(0, 1);
    
    int attempts = 0;
    const int maxAttempts = 1000;
    
    while (attempts < maxAttempts) {
        int x = distPos(gen);
        int y = distPos(gen);
        bool vertical = distDir(gen) == 1;
        
        if (placeShip(x, y, shipSize, vertical)) {
            return true;
        }
        attempts++;
    }
    
    return false;
}

void Field::placeAllShipsAuto() {
    reset();
    
    // 1 четырёхпалубный
    placeShipAuto(4);
    
    // 2 трёхпалубных
    for (int i = 0; i < 2; i++) {
        placeShipAuto(3);
    }
    
    // 3 двухпалубных
    for (int i = 0; i < 3; i++) {
        placeShipAuto(2);
    }
    
    // 4 однопалубных
    for (int i = 0; i < 4; i++) {
        placeShipAuto(1);
    }
}

Field::AttackResult Field::attack(int x, int y) {
    if (!isValidPosition(x, y)) {
        return AttackResult::Invalid;
    }
    
    Cell& cell = cells[y][x];
    
    switch (cell.getState()) {
        case CellState::Empty:
        case CellState::Blocked:
            cell.setState(CellState::Miss);
            return AttackResult::Miss;
            
        case CellState::Ship: {
            cell.setState(CellState::Hit);
            int shipId = cell. getShipId();
            
            // находим корабль и регистрируем попадание
            for (auto& ship : ships) {
                if (ship->getId() == shipId) {
                    ship->hit();
                    if (ship->isDestroyed()) {
                        markDestroyedShipCells(*ship);
                        destroyedShipsCount++;
                        return AttackResult::Destroyed;
                    }
                    return AttackResult::Hit;
                }
            }
            return AttackResult::Hit;
        }
        
        case CellState::Miss:
        case CellState::Hit:
        case CellState::Destroyed:
            return AttackResult::AlreadyHit;
            
        default:
            return AttackResult::Invalid;
    }
}

void Field::draw(bool hideShips) const {
    std::cout << "   ";
    for (int x = 0; x < size; x++) {
        std::cout << static_cast<char>('A' + x) << " ";
    }
    std::cout << std::endl;
    
    for (int y = 0; y < size; y++) {
        std::cout << std::setw(2) << (y + 1) << " ";
        for (int x = 0; x < size; x++) {
            const Cell& cell = cells[y][x];
            
            if (hideShips && (cell.getState() == CellState::Ship || 
                             cell.getState() == CellState::Blocked)) {
                std::cout << "~ ";
            } else if (hideShips && cell. getState() == CellState::Blocked) {
                std::cout << "~ ";
            } else {
                std::cout << cell.getSymbol() << " ";
            }
        }
        std::cout << std::endl;
    }
}

void Field::drawWithColors(bool hideShips) const {
    // ANSI цвета
    const std::string RESET = "\033[0m";
    const std::string BLUE = "\033[34m";
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string CYAN = "\033[36m";
    
    // заголовок
    std::cout << CYAN << "   ";
    for (int x = 0; x < size; x++) {
        std::cout << static_cast<char>('A' + x) << " ";
    }
    std::cout << RESET << std::endl;
    
    // поле
    for (int y = 0; y < size; y++) {
        std::cout << CYAN << std::setw(2) << (y + 1) << " " << RESET;
        for (int x = 0; x < size; x++) {
            const Cell& cell = cells[y][x];
            
            if (hideShips && (cell.getState() == CellState::Ship || 
                             cell.getState() == CellState::Blocked)) {
                std::cout << BLUE << "~ " << RESET;
            } else {
                switch (cell.getState()) {
                    case CellState::Empty:
                        std::cout << BLUE << "~ " << RESET;
                        break;
                    case CellState::Ship:
                        std::cout << GREEN << "O " << RESET;
                        break;
                    case CellState::Miss:
                        std::cout << YELLOW << "* " << RESET;
                        break;
                    case CellState::Hit:
                        std::cout << RED << "X " << RESET;
                        break;
                    case CellState::Destroyed:
                        std::cout << RED << "# " << RESET;
                        break;
                    case CellState::Blocked:
                        std::cout << BLUE << ".  " << RESET;
                        break;
                    default:
                        std::cout << "? ";
                }
            }
        }
        std::cout << std::endl;
    }
}

void Field::reset() {
    ships.clear();
    destroyedShipsCount = 0;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            cells[y][x] = Cell(x, y);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Field& field) {
    os << "   ";
    for (int x = 0; x < field.size; x++) {
        os << static_cast<char>('A' + x) << " ";
    }
    os << std::endl;
    
    for (int y = 0; y < field.size; y++) {
        os << std::setw(2) << (y + 1) << " ";
        for (int x = 0; x < field.size; x++) {
            os << field.cells[y][x] << " ";
        }
        os << std::endl;
    }
    return os;
}