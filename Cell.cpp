#include "Cell.h"

// конструктор по умолчанию
Cell::Cell() : GameObject(0, 0), state(CellState::Empty), shipId(-1) {}

// конструктор с координатами
Cell::Cell(int x, int y) : GameObject(x, y), state(CellState::Empty), shipId(-1) {}

// конструктор с полной инициализацией
Cell::Cell(int x, int y, CellState state, int shipId) 
    : GameObject(x, y), state(state), shipId(shipId) {}

// конструктор копирования
Cell::Cell(const Cell& other) 
    : GameObject(other.x, other.y), state(other.state), shipId(other. shipId) {}

// оператор присваивания
Cell& Cell::operator=(const Cell& other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
        state = other.state;
        shipId = other.shipId;
    }
    return *this;
}

// отрисовка клетки
void Cell::draw() const {
    std::cout << getSymbol();
}

// получение символа для отображения
char Cell::getSymbol() const {
    switch (state) {
        case CellState::Empty:     return '~';
        case CellState::Ship:      return 'O';
        case CellState::Miss:      return '*';
        case CellState::Hit:       return 'X';
        case CellState::Destroyed: return '#';
        case CellState::Blocked:   return '.';
        default:                   return '?';
    }
}

std::ostream& operator<<(std::ostream& os, const Cell& cell) {
    os << cell.getSymbol();
    return os;
}