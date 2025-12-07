#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "Field.h"
#include "Player.h"
#include "Game.h"
#include <memory>
#include <string>

// константы для отрисовки
namespace GameConfig {
    const int CELL_SIZE = 35;
    const int FIELD_SIZE = 10;
    const int MARGIN = 50;
    const int FIELD_GAP = 100;
    const int WINDOW_WIDTH = MARGIN * 3 + FIELD_SIZE * CELL_SIZE * 2 + FIELD_GAP;
    const int WINDOW_HEIGHT = MARGIN * 2 + FIELD_SIZE * CELL_SIZE + 150;
}

enum class GUIState {
    MainMenu,
    PlacingShips,
    Playing,
    GameOver
};

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color clickColor;
    bool isHovered;
    bool isClicked;

public:
    Button();
    Button(float x, float y, float width, float height, 
           const std::string& buttonText, const sf::Font& font);
    
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setText(const std::string& buttonText, const sf::Font& font);
    void setColors(sf::Color normal, sf::Color hover, sf::Color click);
    
    void update(const sf::Vector2i& mousePos);
    bool isPressed(const sf::Event& event, const sf::Vector2i& mousePos);
    void draw(sf::RenderWindow& window);
    
    bool contains(const sf::Vector2i& point) const;
};

// класс для отображения игрового поля
class FieldRenderer {
private:
    float offsetX, offsetY;
    float cellSize;
    bool hideShips;
    std::string title;
    sf::Font& font;
    
    sf::Color getColorForState(CellState state, bool hide) const;
    
public:
    FieldRenderer(float x, float y, float size, const std::string& title, 
                  sf::Font& font, bool hideShips = false);
    
    void draw(sf::RenderWindow& window, const Field& field);
    void drawGrid(sf::RenderWindow& window);
    void drawLabels(sf::RenderWindow& window);
    void drawShipPreview(sf::RenderWindow& window, int x, int y, 
                         int size, bool vertical, bool canPlace);
    
    // преобразование координат мыши в координаты поля
    std::pair<int, int> getFieldCoords(const sf::Vector2i& mousePos) const;
    bool containsPoint(const sf::Vector2i& point) const;
    
    void setHideShips(bool hide) { hideShips = hide; }
};

// главный класс графического интерфейса
class GameGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    // состояние игры
    GUIState state;
    std::unique_ptr<HumanPlayer> player;
    std::unique_ptr<ComputerPlayer> computer;
    
    // рендереры полей
    std::unique_ptr<FieldRenderer> playerFieldRenderer;
    std::unique_ptr<FieldRenderer> computerFieldRenderer;
    
    // элементы интерфейса
    std::vector<Button> menuButtons;
    Button rotateButton;
    Button autoPlaceButton;
    Button startGameButton;
    Button restartButton;
    Button menuButton;
    
    // состояние расстановки кораблей
    struct ShipPlacement {
        int shipsToPlace[4] = {1, 2, 3, 4}; // 4-палубный, 3-палубные, 2-палубные, 1-палубные
        int currentShipSize = 4;
        bool isVertical = true;
        
        int getNextShipSize() const {
            for (int i = 0; i < 4; i++) {
                if (shipsToPlace[i] > 0) {
                    return 4 - i;
                }
            }
            return 0;
        }
        
        bool allPlaced() const {
            return shipsToPlace[0] == 0 && shipsToPlace[1] == 0 && 
                   shipsToPlace[2] == 0 && shipsToPlace[3] == 0;
        }
        
        void reset() {
            shipsToPlace[0] = 1;
            shipsToPlace[1] = 2;
            shipsToPlace[2] = 3;
            shipsToPlace[3] = 4;
            currentShipSize = 4;
            isVertical = true;
        }
    } shipPlacement;
    
    // состояние игры
    bool isPlayerTurn;
    bool gameOver;
    std::string winnerName;
    std::string statusMessage;
    sf::Clock computerThinkClock;
    bool computerThinking;
    
    // статистика
    int playerShots;
    int playerHits;
    int computerShots;
    int computerHits;
    
    // приватные методы
    void initializeMenuButtons();
    void initializeGameButtons();
    void handleMenuEvents(const sf::Event& event);
    void handlePlacingEvents(const sf::Event& event);
    void handlePlayingEvents(const sf::Event& event);
    void handleGameOverEvents(const sf::Event& event);
    
    void updatePlacing();
    void updatePlaying();
    
    void drawMenu();
    void drawPlacing();
    void drawPlaying();
    void drawGameOver();
    void drawStatus();
    void drawShipsToPlace();
    void drawStatistics();
    
    void startNewGame();
    void computerMove();
    void checkGameOver();
    
public:
    GameGUI();
    ~GameGUI() = default;
    
    bool initialize();
    void run();
};

#endif