#ifndef PLAYER_H
#define PLAYER_H

#include "Field.h"
#include <string>
#include <vector>
#include <utility>

class AbstractPlayer {
protected:
    std::string name;
    Field field;
    int shotsCount;
    int hitsCount;
    
public:
    AbstractPlayer();
    explicit AbstractPlayer(const std::string& name);
    virtual ~AbstractPlayer() = default;

    virtual std::pair<int, int> makeMove() = 0;
    virtual void placeShips() = 0;
    virtual bool isHuman() const = 0;

    Field& getField() { return field; }
    const Field& getField() const { return field; }
    std::string getName() const { return name; }
    int getShotsCount() const { return shotsCount; }
    int getHitsCount() const { return hitsCount; }
    void incrementShots() { shotsCount++; }
    void incrementHits() { hitsCount++; }
    double getAccuracy() const;
    
    bool hasLost() const { return field.allShipsDestroyed(); }
};

class HumanPlayer : public AbstractPlayer {
public:
    HumanPlayer();
    explicit HumanPlayer(const std::string& name);
    
    std::pair<int, int> makeMove() override;
    void placeShips() override;
    bool isHuman() const override { return true; }
    
private:
    std::pair<int, int> parseInput(const std::string& input) const;
};

class ComputerPlayer : public AbstractPlayer {
private:
    std::vector<std::pair<int, int>> availableCells;
    std::vector<std::pair<int, int>> priorityCells;
    int lastHitX, lastHitY;
    bool isHunting;
    
    void initAvailableCells();
    void addPriorityCells(int x, int y);
    
public:
    ComputerPlayer();
    explicit ComputerPlayer(const std::string& name);
    
    std::pair<int, int> makeMove() override;
    void placeShips() override;
    bool isHuman() const override { return false; }
    
    void onHit(int x, int y);
    void onDestroyed();
    void reset();
};

#endif