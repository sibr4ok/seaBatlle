#ifndef SHIP_H
#define SHIP_H

#include "Cell.h"
#include <vector>
#include <string>

// абстрактный базовый класс корабля
class AbstractShip : public GameObject {
protected:
    int size;
    int hitCount;
    bool isVertical;
    std::string name;
    static int totalShipsCreated;  // статический счетчик кораблей
    
public:
    // конструкторы
    AbstractShip();
    AbstractShip(int size);
    AbstractShip(int size, int x, int y, bool vertical = true);
    
    // виртуальный деструктор
    virtual ~AbstractShip() = default;
    
    // чисто виртуальные функции
    virtual bool isDestroyed() const = 0;
    virtual void hit() = 0;
    virtual std::string getTypeName() const = 0;
    
    // реализация виртуальных функций из GameObject
    void draw() const override;
    char getSymbol() const override;
    
    // геттеры
    int getSize() const { return size; }
    int getHitCount() const { return hitCount; }
    bool getIsVertical() const { return isVertical; }
    std::string getName() const { return name; }
    
    // сеттеры
    void setVertical(bool vertical) { isVertical = vertical; }
    void setName(const std::string& n) { name = n; }
    
    // статический метод
    static int getTotalShipsCreated() { return totalShipsCreated; }
    
    // получение координат всех палуб
    std::vector<std::pair<int, int>> getDecksCoordinates() const;
};

// конкретный класс корабля
class Ship : public AbstractShip {
private:
    int id;
    static int nextId;
    
public:
    // перегрузка конструкторов
    Ship();
    Ship(int size);
    Ship(int size, int x, int y);
    Ship(int size, int x, int y, bool vertical);
    
    // конструктор копирования
    Ship(const Ship& other);
    
    // деструктор
    ~Ship() override;
    
    // реализация чисто виртуальных функций
    bool isDestroyed() const override;
    void hit() override;
    std::string getTypeName() const override;
    
    // геттеры
    int getId() const { return id; }
    
    // перегрузка операторов
    Ship& operator=(const Ship& other);
    bool operator==(const Ship& other) const;
    
    friend std::ostream& operator<<(std::ostream& os, const Ship& ship);
};

// специализированные классы кораблей (множественный уровень наследования)

// линкор (4 палубы)
class Battleship : public Ship {
public:
    Battleship();
    Battleship(int x, int y, bool vertical = true);
    std::string getTypeName() const override;
};

// крейсер (3 палубы)
class Cruiser : public Ship {
public:
    Cruiser();
    Cruiser(int x, int y, bool vertical = true);
    std::string getTypeName() const override;
};

// эсминец (2 палубы)
class Destroyer : public Ship {
public:
    Destroyer();
    Destroyer(int x, int y, bool vertical = true);
    std::string getTypeName() const override;
};

// катер (1 палуба)
class Boat : public Ship {
public:
    Boat();
    Boat(int x, int y);
    std::string getTypeName() const override;
};

#endif