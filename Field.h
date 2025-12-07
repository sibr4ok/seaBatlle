#ifndef FIELD_H
#define FIELD_H

#include "Cell.h"
#include "Ship.h"
#include <vector>
#include <memory>

// размер поля по умолчанию
const int DEFAULT_FIELD_SIZE = 10;

// класс игрового поля
class Field {
private:
    int size;
    std::vector<std::vector<Cell>> cells;
    std::vector<std::unique_ptr<Ship>> ships;
    int destroyedShipsCount;
    
    // приватные методы
    bool isValidPosition(int x, int y) const;
    void markSurroundingCells(const Ship& ship);
    void markDestroyedShipCells(const Ship& ship);
    
public:
    // конструкторы
    Field();
    explicit Field(int size);
    
    Field(const Field& other);
    
    Field& operator=(const Field& other);
    
    ~Field() = default;
    
    // методы для работы с кораблями
    bool canPlaceShip(int x, int y, int shipSize, bool vertical) const;
    bool placeShip(int x, int y, int shipSize, bool vertical);
    bool placeShipAuto(int shipSize);
    void placeAllShipsAuto();
    
    // шаблонный метод для размещения конкретного типа корабля
    template<typename T>
    bool placeSpecificShip(int x, int y, bool vertical);
    
    // методы для атаки
    enum class AttackResult { Miss, Hit, Destroyed, AlreadyHit, Invalid };
    AttackResult attack(int x, int y);
    
    // отображение
    void draw(bool hideShips = false) const;
    void drawWithColors(bool hideShips = false) const;
    
    // геттеры
    int getSize() const { return size; }
    int getShipsCount() const { return ships.size(); }
    int getDestroyedShipsCount() const { return destroyedShipsCount; }
    int getAliveShipsCount() const { return ships. size() - destroyedShipsCount; }
    bool allShipsDestroyed() const { return destroyedShipsCount >= static_cast<int>(ships.size()); }
    Cell& getCell(int x, int y) { return cells[y][x]; }
    const Cell& getCell(int x, int y) const { return cells[y][x]; }
    
    // сброс поля
    void reset();
    
    friend std::ostream& operator<<(std::ostream& os, const Field& field);
};

template<typename T>
bool Field::placeSpecificShip(int x, int y, bool vertical) {
    T tempShip;
    int shipSize = tempShip.getSize();
    
    if (!canPlaceShip(x, y, shipSize, vertical)) {
        return false;
    }
    
    auto ship = std::make_unique<T>(x, y, vertical);
    
    // помечаем клетки как занятые кораблём
    auto coords = ship->getDecksCoordinates();
    for (const auto& coord : coords) {
        cells[coord.second][coord.first]. setState(CellState::Ship);
        cells[coord.second][coord.first].setShipId(ship->getId());
    }
    
    // помечаем окружающие клетки
    markSurroundingCells(*ship);
    
    ships.push_back(std::move(ship));
    return true;
}

#endif