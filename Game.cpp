#include "Game.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

// инициализация статических членов
int Game::gamesPlayed = 0;
int Game::player1Wins = 0;
int Game::player2Wins = 0;

// конструкторы

Game::Game() : mode(GameMode::PlayerVsComputer), state(GameState::NotStarted), turnCount(0) {
    player1 = std::make_unique<HumanPlayer>("Игрок");
    player2 = std::make_unique<ComputerPlayer>("Компьютер");
    currentPlayer = player1.get();
    opponent = player2. get();
}

Game::Game(GameMode mode) : mode(mode), state(GameState::NotStarted), turnCount(0) {
    if (mode == GameMode::PlayerVsComputer) {
        player1 = std::make_unique<HumanPlayer>("Игрок");
        player2 = std::make_unique<ComputerPlayer>("Компьютер");
    } else {
        player1 = std::make_unique<ComputerPlayer>("Компьютер 1");
        player2 = std::make_unique<ComputerPlayer>("Компьютер 2");
    }
    currentPlayer = player1.get();
    opponent = player2.get();
}

Game::Game(const std::string& player1Name, const std::string& player2Name) 
    : mode(GameMode::PlayerVsComputer), state(GameState::NotStarted), turnCount(0) {
    player1 = std::make_unique<HumanPlayer>(player1Name);
    player2 = std::make_unique<ComputerPlayer>(player2Name);
    currentPlayer = player1.get();
    opponent = player2. get();
}

// методы

void Game::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Game::initialize() {
    state = GameState::PlacingShips;
    
    std::cout << "\n========================================\n";
    std::cout << "         МОРСКОЙ БОЙ - НАЧАЛО ИГРЫ\n";
    std::cout << "========================================\n\n";
    
    // расстановка кораблей первого игрока
    std::cout << ">>> " << player1->getName() << ", расставьте корабли <<<\n";
    player1->placeShips();
    
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
    
    // расстановка кораблей второго игрока
    if (! player2->isHuman()) {
        std::cout << "\n" << player2->getName() << " расставляет корабли.. .\n";
        player2->placeShips();
        std::cout << "Корабли компьютера расставлены!\n";
    } else {
        clearScreen();
        std::cout << ">>> " << player2->getName() << ", расставьте корабли <<<\n";
        player2->placeShips();
    }
    
    state = GameState::InProgress;
}

void Game::displayFields() const {
    const std::string CYAN = "\033[36m";
    const std::string RESET = "\033[0m";
    
    std::cout << "\n" << CYAN << "╔════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << CYAN << "║" << RESET << "        ВАШЕ ПОЛЕ                    ПОЛЕ ПРОТИВНИКА        " << CYAN << "║" << RESET << "\n";
    std::cout << CYAN << "╚════════════════════════════════════════════════════════════╝" << RESET << "\n\n";
    
    // заголовки
    std::cout << "   A B C D E F G H I J       A B C D E F G H I J\n";
    
    // поля
    for (int y = 0; y < 10; y++) {
        // левое поле (игрока - показываем корабли)
        std::cout << std::setw(2) << (y + 1) << " ";
        for (int x = 0; x < 10; x++) {
            const Cell& cell = player1->getField(). getCell(x, y);
            
            const std::string BLUE = "\033[34m";
            const std::string GREEN = "\033[32m";
            const std::string RED = "\033[31m";
            const std::string YELLOW = "\033[33m";
            
            switch (cell.getState()) {
                case CellState::Empty:
                case CellState::Blocked:
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
                default:
                    std::cout << "?  ";
            }
        }
        
        std::cout << "    ";
        
        // правое поле (противника - скрываем корабли)
        std::cout << std::setw(2) << (y + 1) << " ";
        for (int x = 0; x < 10; x++) {
            const Cell& cell = player2->getField().getCell(x, y);
            
            const std::string BLUE = "\033[34m";
            const std::string RED = "\033[31m";
            const std::string YELLOW = "\033[33m";
            
            switch (cell. getState()) {
                case CellState::Empty:
                case CellState::Blocked:
                case CellState::Ship:
                    std::cout << BLUE << "~ " << RESET;
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
                default:
                    std::cout << "? ";
            }
        }
        std::cout << "\n";
    }
}

void Game::displayStats() const {
    std::cout << "\n┌─────────────────────────────────────┐\n";
    std::cout << "│ Ход: " << std::setw(3) << turnCount << "                              │\n";
    std::cout << "├─────────────────────────────────────┤\n";
    std::cout << "│ " << player1->getName() << ": живых кораблей - " 
              << player1->getField().getAliveShipsCount() << "         │\n";
    std::cout << "│ " << player2->getName() << ": живых кораблей - " 
              << player2->getField().getAliveShipsCount() << "   │\n";
    std::cout << "└─────────────────────────────────────┘\n";
}

void Game::switchTurn() {
    std::swap(currentPlayer, opponent);
}

void Game::processAttack(int x, int y, AbstractPlayer* attacker, AbstractPlayer* target) {
    attacker->incrementShots();
    
    Field::AttackResult result = target->getField().attack(x, y);
    
    char colLetter = 'A' + x;
    std::cout << attacker->getName() << " стреляет: " << colLetter << (y + 1) << " - ";
    
    switch (result) {
        case Field::AttackResult::Miss:
            std::cout << "\033[33mМимо!\033[0m\n";
            switchTurn();
            break;
            
        case Field::AttackResult::Hit:
            std::cout << "\033[31mПопадание!\033[0m\n";
            attacker->incrementHits();
            if (! attacker->isHuman()) {
                dynamic_cast<ComputerPlayer*>(attacker)->onHit(x, y);
            }
            break;
            
        case Field::AttackResult::Destroyed:
            std::cout << "\033[31;1mУбит!\033[0m\n";
            attacker->incrementHits();
            if (!attacker->isHuman()) {
                dynamic_cast<ComputerPlayer*>(attacker)->onDestroyed();
            }
            break;
            
        case Field::AttackResult::AlreadyHit:
            std::cout << "Вы уже стреляли сюда!  Повторите выстрел.\n";
            break;
            
        case Field::AttackResult::Invalid:
            std::cout << "Неверные координаты!\n";
            break;
    }
}

void Game::start() {
    initialize();
}

void Game::run() {
    std::cout << "\n\n========================================\n";
    std::cout << "            НАЧАЛО БОЯ!\n";
    std::cout << "========================================\n";
    
    while (state == GameState::InProgress) {
        turnCount++;
        
        if (mode == GameMode::PlayerVsComputer) {
            clearScreen();
            displayFields();
            displayStats();
        }
        
        // ход текущего игрока
        AbstractPlayer* attacker = currentPlayer;
        AbstractPlayer* target = opponent;
        
        if (attacker->isHuman()) {
            std::cout << "\n>>> Ваш ход <<<\n";
        } else {
            std::cout << "\n>>> Ход " << attacker->getName() << " <<<\n";
            if (mode == GameMode::ComputerVsComputer) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        
        auto coords = attacker->makeMove();
        processAttack(coords. first, coords.second, attacker, target);
        
        // проверка победы
        if (target->hasLost()) {
            state = GameState::Finished;
            gamesPlayed++;
            
            if (attacker == player1. get()) {
                player1Wins++;
            } else {
                player2Wins++;
            }
            
            clearScreen();
            displayFields();
            
            std::cout << "\n\033[32;1m========================================\033[0m\n";
            std::cout << "\033[32;1m     ПОБЕДА!  " << attacker->getName() << " выиграл!\033[0m\n";
            std::cout << "\033[32;1m========================================\033[0m\n\n";
            
            std::cout << "Статистика игры:\n";
            std::cout << "  Всего ходов: " << turnCount << "\n";
            std::cout << "  " << player1->getName() << " - выстрелов: " << player1->getShotsCount()
                      << ", попаданий: " << player1->getHitsCount() 
                      << ", точность: " << std::fixed << std::setprecision(1) 
                      << player1->getAccuracy() << "%\n";
            std::cout << "  " << player2->getName() << " - выстрелов: " << player2->getShotsCount()
                      << ", попаданий: " << player2->getHitsCount() 
                      << ", точность: " << std::fixed << std::setprecision(1) 
                      << player2->getAccuracy() << "%\n";
        }
        
        if (mode == GameMode::PlayerVsComputer && ! currentPlayer->isHuman() && 
            state == GameState::InProgress) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

void Game::reset() {
    player1->getField().reset();
    player2->getField().reset();
    
    if (auto* comp = dynamic_cast<ComputerPlayer*>(player2.get())) {
        comp->reset();
    }
    if (auto* comp = dynamic_cast<ComputerPlayer*>(player1.get())) {
        comp->reset();
    }
    
    currentPlayer = player1. get();
    opponent = player2.get();
    state = GameState::NotStarted;
    turnCount = 0;
}

void Game::resetStats() {
    gamesPlayed = 0;
    player1Wins = 0;
    player2Wins = 0;
}

void Game::showMainMenu() {
    const std::string CYAN = "\033[36m";
    const std::string YELLOW = "\033[33m";
    const std::string RESET = "\033[0m";
    
    std::cout << CYAN;
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════╗
    ║                                                       ║
    ║     __  __  ___  ___  ___  _  _  ___  ___             ║
    ║    |  \/  |/ _ \| _ \/ __|| |/ // _ \|_ _|            ║
    ║    | |\/| | (_) |   /\__ \|   <| (_) || |             ║
    ║    |_|  |_|\___/|_|_\|___/|_|\_\\___/|___|            ║
    ║                                                       ║
    ║                    БОЙ                                ║
    ║                                                       ║
    ╠═══════════════════════════════════════════════════════╣
    ║                                                       ║
    )" << RESET;
    
    std::cout << CYAN << "    ║" << RESET << YELLOW << "     1. Игрок против Компьютера                    " << CYAN << "║" << RESET << "\n";
    std::cout << CYAN << "    ║" << RESET << YELLOW << "     2. Компьютер против Компьютера                " << CYAN << "║" << RESET << "\n";
    std::cout << CYAN << "    ║" << RESET << YELLOW << "     3.  Статистика                                 " << CYAN << "║" << RESET << "\n";
    std::cout << CYAN << "    ║" << RESET << YELLOW << "     0. Выход                                      " << CYAN << "║" << RESET << "\n";
    
    std::cout << CYAN;
    std::cout << R"(    ║                                                       ║
    ╚═══════════════════════════════════════════════════════╝
    )" << RESET;
}

GameMode Game::selectGameMode() {
    int choice;
    std::cout << "\n    Ваш выбор: ";
    std::cin >> choice;
    
    switch (choice) {
        case 1: return GameMode::PlayerVsComputer;
        case 2: return GameMode::ComputerVsComputer;
        default: return GameMode::PlayerVsComputer;
    }
}