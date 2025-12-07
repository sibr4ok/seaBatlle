#ifndef CELL_H
#define CELL_H

#include <iostream>

enum class CellState {
    Empty,     
    Ship,      
    Miss,       
    Hit,        
    Destroyed, 
    Blocked
};

class GameObject {
protected:
    int x, y;
    
public:
    GameObject() : x(0), y(0) {}
    GameObject(int x, int y) : x(x), y(y) {}
    virtual ~GameObject() = default;
    
    virtual void draw() const = 0;
    virtual char getSymbol() const = 0;
    
    int getX() const { return x; }
    int getY() const { return y; }
    
    void setPosition(int newX, int newY) { x = newX; y = newY; }
};

class Cell : public GameObject {
private:
    CellState state;
    int shipId;
    
public:
    Cell();
    
    Cell(int x, int y);
    
    Cell(int x, int y, CellState state, int shipId = -1);
    
    Cell(const Cell& other);
    
    Cell& operator=(const Cell& other);
    
    ~Cell() override = default;
    
    void draw() const override;
    char getSymbol() const override;
    
    CellState getState() const { return state; }
    void setState(CellState newState) { state = newState; }
    int getShipId() const { return shipId; }
    void setShipId(int id) { shipId = id; }
    
    bool isEmpty() const { return state == CellState::Empty; }
    bool hasShip() const { return state == CellState::Ship; }
    bool isHit() const { return state == CellState::Hit || state == CellState::Destroyed; }
    
    friend std::ostream& operator<<(std::ostream& os, const Cell& cell);
};

#endif