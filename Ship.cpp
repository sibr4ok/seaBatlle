#include "Ship.h"
#include <iostream>

int AbstractShip::totalShipsCreated = 0;
int Ship::nextId = 0;

// AbstractShip

AbstractShip::AbstractShip() 
    : GameObject(0, 0), size(1), hitCount(0), isVertical(true), name("Ship") {
    totalShipsCreated++;
}

AbstractShip::AbstractShip(int size) 
    : GameObject(0, 0), size(size), hitCount(0), isVertical(true), name("Ship") {
    totalShipsCreated++;
}

AbstractShip::AbstractShip(int size, int x, int y, bool vertical) 
    : GameObject(x, y), size(size), hitCount(0), isVertical(vertical), name("Ship") {
    totalShipsCreated++;
}

void AbstractShip::draw() const {
    std::cout << "[" << name << " (" << size << " палуб)]";
}

char AbstractShip::getSymbol() const {
    return isDestroyed() ? '#' : 'O';
}

std::vector<std::pair<int, int>> AbstractShip::getDecksCoordinates() const {
    std::vector<std::pair<int, int>> coords;
    for (int i = 0; i < size; i++) {
        if (isVertical) {
            coords. push_back({x, y + i});
        } else {
            coords.push_back({x + i, y});
        }
    }
    return coords;
}

// Ship

Ship::Ship() : AbstractShip(), id(nextId++) {}

Ship::Ship(int size) : AbstractShip(size), id(nextId++) {}

Ship::Ship(int size, int x, int y) : AbstractShip(size, x, y, true), id(nextId++) {}

Ship::Ship(int size, int x, int y, bool vertical) 
    : AbstractShip(size, x, y, vertical), id(nextId++) {}

Ship::Ship(const Ship& other) 
    : AbstractShip(other.size, other.x, other.y, other.isVertical), id(nextId++) {
    hitCount = other.hitCount;
    name = other.name;
}

Ship::~Ship() {
    // деструктор
}

bool Ship::isDestroyed() const {
    return hitCount >= size;
}

void Ship::hit() {
    if (hitCount < size) {
        hitCount++;
    }
}

std::string Ship::getTypeName() const {
    switch (size) {
        case 4: return "Линкор";
        case 3: return "Крейсер";
        case 2: return "Эсминец";
        case 1: return "Катер";
        default: return "Корабль";
    }
}

Ship& Ship::operator=(const Ship& other) {
    if (this != &other) {
        size = other.size;
        x = other.x;
        y = other.y;
        isVertical = other.isVertical;
        hitCount = other.hitCount;
        name = other.name;
    }
    return *this;
}

bool Ship::operator==(const Ship& other) const {
    return id == other.id;
}

std::ostream& operator<<(std::ostream& os, const Ship& ship) {
    os << ship.getTypeName() << " [" << ship.size << " палуб, попаданий: " 
       << ship. hitCount << "/" << ship.size << "]";
    return os;
}

// Battleship

Battleship::Battleship() : Ship(4) {
    name = "Линкор";
}

Battleship::Battleship(int x, int y, bool vertical) : Ship(4, x, y, vertical) {
    name = "Линкор";
}

std::string Battleship::getTypeName() const {
    return "Линкор";
}

// Cruiser

Cruiser::Cruiser() : Ship(3) {
    name = "Крейсер";
}

Cruiser::Cruiser(int x, int y, bool vertical) : Ship(3, x, y, vertical) {
    name = "Крейсер";
}

std::string Cruiser::getTypeName() const {
    return "Крейсер";
}

// Destroyer

Destroyer::Destroyer() : Ship(2) {
    name = "Эсминец";
}

Destroyer::Destroyer(int x, int y, bool vertical) : Ship(2, x, y, vertical) {
    name = "Эсминец";
}

std::string Destroyer::getTypeName() const {
    return "Эсминец";
}

// Boat

Boat::Boat() : Ship(1) {
    name = "Катер";
}

Boat::Boat(int x, int y) : Ship(1, x, y, true) {
    name = "Катер";
}

std::string Boat::getTypeName() const {
    return "Катер";
}