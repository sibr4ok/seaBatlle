#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include <memory>

// режимы игры
enum class GameMode {
    PlayerVsComputer,
    ComputerVsComputer
};

// состояния игры
enum class GameState {
    NotStarted,
    PlacingShips,
    InProgress,
    Finished
};

// главный класс игры
class Game {
private:
    std::unique_ptr<AbstractPlayer> player1;
    std::unique_ptr<AbstractPlayer> player2;
    AbstractPlayer* currentPlayer;
    AbstractPlayer* opponent;
    GameMode mode;
    GameState state;
    int turnCount;
    
    // статические члены
    static int gamesPlayed;
    static int player1Wins;
    static int player2Wins;
    
    // приватные методы
    void switchTurn();
    void displayFields() const;
    void displayStats() const;
    void processAttack(int x, int y, AbstractPlayer* attacker, AbstractPlayer* target);
    void clearScreen() const;
    
public:
    // конструкторы
    Game();
    Game(GameMode mode);
    Game(const std::string& player1Name, const std::string& player2Name);
    
    // запрет копирования
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    
    // деструктор
    ~Game() = default;
    
    // основные методы
    void initialize();
    void start();
    void run();
    void reset();
    
    // геттеры
    GameState getState() const { return state; }
    GameMode getMode() const { return mode; }
    int getTurnCount() const { return turnCount; }
    
    // статические методы
    static int getGamesPlayed() { return gamesPlayed; }
    static int getPlayer1Wins() { return player1Wins; }
    static int getPlayer2Wins() { return player2Wins; }
    static void resetStats();
    
    // отображение меню
    static void showMainMenu();
    static GameMode selectGameMode();
};

#endif