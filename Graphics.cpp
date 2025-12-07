#include "Graphics.h"
#include <iostream>

// вспомогательная функция для исправления кодировки (UTF-8 -> SFML String)
sf::String fromUtf8(const std::string& str) {
    return sf::String::fromUtf8(str.begin(), str.end());
}

// button

Button::Button() : isHovered(false), isClicked(false) {
    normalColor = sf::Color(200, 200, 200);
    hoverColor = sf::Color(180, 180, 180);
    clickColor = sf::Color(150, 150, 150);
    
    shape.setFillColor(normalColor);
    shape.setOutlineThickness(1);
    shape.setOutlineColor(sf::Color::Black);
}

Button::Button(float x, float y, float width, float height, 
       const std::string& buttonText, const sf::Font& font) : Button() {
    setPosition(x, y);
    setSize(width, height);
    setText(buttonText, font);
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    
    // центрируем текст
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        x + (shape.getSize().x - textBounds.width) / 2.0f - textBounds.left,
        y + (shape.getSize().y - textBounds.height) / 2.0f - textBounds.top
    );
}

void Button::setSize(float width, float height) {
    shape.setSize(sf::Vector2f(width, height));
    setPosition(shape.getPosition().x, shape.getPosition().y); // пересчитываем позицию текста
}

void Button::setText(const std::string& buttonText, const sf::Font& font) {
    text.setFont(font);
    text.setString(fromUtf8(buttonText));
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::Black);
    setPosition(shape.getPosition().x, shape.getPosition().y);
}

void Button::setColors(sf::Color normal, sf::Color hover, sf::Color click) {
    normalColor = normal;
    hoverColor = hover;
    clickColor = click;
}

void Button::update(const sf::Vector2i& mousePos) {
    isHovered = contains(mousePos);
    if (isHovered) {
        shape.setFillColor(sf::Mouse::isButtonPressed(sf::Mouse::Left) ? clickColor : hoverColor);
    } else {
        shape.setFillColor(normalColor);
    }
}

bool Button::isPressed(const sf::Event& event, const sf::Vector2i& mousePos) {
    if (event.type == sf::Event::MouseButtonReleased && 
        event.mouseButton.button == sf::Mouse::Left) {
        return contains(mousePos);
    }
    return false;
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::contains(const sf::Vector2i& point) const {
    return shape.getGlobalBounds().contains(static_cast<float>(point.x), static_cast<float>(point.y));
}

// FieldRenderer

FieldRenderer::FieldRenderer(float x, float y, float size, const std::string& title, 
              sf::Font& font, bool hideShips)
    : offsetX(x), offsetY(y), cellSize(size), hideShips(hideShips), title(title), font(font) {
}

sf::Color FieldRenderer::getColorForState(CellState state, bool hide) const {
    switch (state) {
        case CellState::Empty:     return sf::Color(230, 240, 255);
        case CellState::Blocked:   return sf::Color(230, 240, 255);
        case CellState::Ship:      return hide ? sf::Color(230, 240, 255) : sf::Color(100, 100, 100);
        case CellState::Miss:      return sf::Color(230, 240, 255);
        case CellState::Hit:       return sf::Color(255, 200, 200);
        case CellState::Destroyed: return sf::Color(255, 150, 150);
        default:                   return sf::Color::White;
    }
}

void FieldRenderer::draw(sf::RenderWindow& window, const Field& field) {
    drawLabels(window);
    
    for (int y = 0; y < GameConfig::FIELD_SIZE; y++) {
        for (int x = 0; x < GameConfig::FIELD_SIZE; x++) {
            const Cell& cell = field.getCell(x, y);
            
            sf::RectangleShape rect(sf::Vector2f(cellSize - 1, cellSize - 1));
            rect.setPosition(offsetX + x * cellSize, offsetY + y * cellSize);
            rect.setFillColor(getColorForState(cell.getState(), hideShips));
            
            if (!hideShips && cell.getState() == CellState::Blocked) {
                sf::CircleShape dot(2);
                dot.setFillColor(sf::Color(150, 150, 200));
                dot.setPosition(
                    rect.getPosition().x + cellSize/2 - 2,
                    rect.getPosition().y + cellSize/2 - 2
                );
                window.draw(rect);
                window.draw(dot);
                continue;
            }

            window.draw(rect);
            
            if (cell.getState() == CellState::Hit || cell.getState() == CellState::Destroyed) {
                sf::Vertex line1[] = {
                    sf::Vertex(sf::Vector2f(rect.getPosition().x + 5, rect.getPosition().y + 5), sf::Color::Red),
                    sf::Vertex(sf::Vector2f(rect.getPosition().x + cellSize - 5, rect.getPosition().y + cellSize - 5), sf::Color::Red)
                };
                sf::Vertex line2[] = {
                    sf::Vertex(sf::Vector2f(rect.getPosition().x + cellSize - 5, rect.getPosition().y + 5), sf::Color::Red),
                    sf::Vertex(sf::Vector2f(rect.getPosition().x + 5, rect.getPosition().y + cellSize - 5), sf::Color::Red)
                };
                window.draw(line1, 2, sf::Lines);
                window.draw(line2, 2, sf::Lines);
            } else if (cell.getState() == CellState::Miss) {
                sf::CircleShape dot(4);
                dot.setFillColor(sf::Color::Black);
                dot.setOrigin(4, 4);
                dot.setPosition(
                    rect.getPosition().x + cellSize/2,
                    rect.getPosition().y + cellSize/2
                );
                window.draw(dot);
            }
        }
    }
}

void FieldRenderer::drawGrid(sf::RenderWindow& window) {}

void FieldRenderer::drawLabels(sf::RenderWindow& window) {
    sf::Text titleText(fromUtf8(title), font, 22);
    titleText.setFillColor(sf::Color::Black);
    sf::FloatRect bounds = titleText.getLocalBounds();
    titleText.setPosition(
        offsetX + (GameConfig::FIELD_SIZE * cellSize - bounds.width) / 2,
        offsetY - 55
    );
    window.draw(titleText);
    
    for (int i = 0; i < GameConfig::FIELD_SIZE; i++) {
        std::string letter;
        letter += static_cast<char>('A' + i);
        sf::Text text(letter, font, 14);
        text.setFillColor(sf::Color::Black);
        text.setPosition(offsetX + i * cellSize + 12, offsetY - 20);
        window.draw(text);
    }

    for (int i = 0; i < GameConfig::FIELD_SIZE; i++) {
        sf::Text text(std::to_string(i + 1), font, 14);
        text. setFillColor(sf::Color::Black);
        text.setPosition(offsetX - 20, offsetY + i * cellSize + 8);
        window.draw(text);
    }
}

void FieldRenderer::drawShipPreview(sf::RenderWindow& window, int x, int y, 
                     int size, bool vertical, bool canPlace) {
    sf::Color color = canPlace ? sf::Color(100, 255, 100, 150) : sf::Color(255, 100, 100, 150);
    
    for (int i = 0; i < size; i++) {
        int cx = vertical ? x : x + i;
        int cy = vertical ? y + i : y;
        
        if (cx >= 0 && cx < 10 && cy >= 0 && cy < 10) {
            sf::RectangleShape rect(sf::Vector2f(cellSize - 1, cellSize - 1));
            rect.setPosition(offsetX + cx * cellSize, offsetY + cy * cellSize);
            rect.setFillColor(color);
            window.draw(rect);
        }
    }
}

std::pair<int, int> FieldRenderer::getFieldCoords(const sf::Vector2i& mousePos) const {
    if (!containsPoint(mousePos)) return {-1, -1};
    
    int x = static_cast<int>((mousePos.x - offsetX) / cellSize);
    int y = static_cast<int>((mousePos.y - offsetY) / cellSize);
    
    return {x, y};
}

bool FieldRenderer::containsPoint(const sf::Vector2i& point) const {
    float width = GameConfig::FIELD_SIZE * cellSize;
    float height = GameConfig::FIELD_SIZE * cellSize;
    
    return point.x >= offsetX && point.x < offsetX + width &&
           point.y >= offsetY && point.y < offsetY + height;
}

// GameGUI 

GameGUI::GameGUI() : state(GUIState::MainMenu), isPlayerTurn(true), gameOver(false), 
                     computerThinking(false), playerShots(0), playerHits(0), 
                     computerShots(0), computerHits(0) {
    
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    
    window.create(sf::VideoMode(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT), 
                 fromUtf8("Морской Бой (SFML)"), sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(60);
}

bool GameGUI::initialize() {
    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") && 
            !font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            std::cerr << "Ошибка: Шрифт не найден!" << std::endl;
            return false;
        }
    }
    
    playerFieldRenderer = std::make_unique<FieldRenderer>(
        GameConfig::MARGIN, 
        40, 
        GameConfig::CELL_SIZE, 
        "",
        font, 
        false 
    );
    
    computerFieldRenderer = std::make_unique<FieldRenderer>(
        GameConfig::MARGIN * 2 + GameConfig::FIELD_SIZE * GameConfig::CELL_SIZE, 
        40, 
        GameConfig::CELL_SIZE, 
        "",
        font, 
        true 
    );
    
    initializeMenuButtons();
    initializeGameButtons();
    
    return true;
}

void GameGUI::initializeMenuButtons() {
    float btnWidth = 300;
    float btnHeight = 50;
    float startY = 200;
    float centerX = GameConfig::WINDOW_WIDTH / 2.0f - btnWidth / 2.0f;
    
    startGameButton = Button(centerX, startY, btnWidth, btnHeight, "Новая игра", font);
    startGameButton.setColors(sf::Color(100, 200, 100), sf::Color(120, 220, 120), sf::Color(80, 180, 80));
    
    Button exitBtn(centerX, startY + 70, btnWidth, btnHeight, "Выход", font);
    exitBtn.setColors(sf::Color(200, 100, 100), sf::Color(220, 120, 120), sf::Color(180, 80, 80));
    
    menuButtons.push_back(startGameButton);
    menuButtons.push_back(exitBtn);
}

void GameGUI::initializeGameButtons() {
    float yPos = GameConfig::WINDOW_HEIGHT - 80;
    
    rotateButton = Button(GameConfig::MARGIN, yPos, 200, 40, "Повернуть (R)", font);
    autoPlaceButton = Button(GameConfig::MARGIN + 220, yPos, 200, 40, "Авто-расстановка", font);
    
    restartButton = Button(GameConfig::WINDOW_WIDTH - 250, yPos, 200, 40, "В меню", font);
}

void GameGUI::startNewGame() {
    player = std::make_unique<HumanPlayer>("Игрок");
    computer = std::make_unique<ComputerPlayer>("Компьютер");
    
    player->getField().reset();
    computer->getField().reset();
    computer->placeShips(); 
    
    shipPlacement.reset();
    
    state = GUIState::PlacingShips;
    gameOver = false;
    isPlayerTurn = true;
    statusMessage = "Расставьте корабли!";
    
    playerShots = 0; playerHits = 0;
    computerShots = 0; computerHits = 0;
}

void GameGUI::run() {
    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
                
            switch (state) {
                case GUIState::MainMenu:
                    handleMenuEvents(event);
                    break;
                case GUIState::PlacingShips:
                    handlePlacingEvents(event);
                    break;
                case GUIState::Playing:
                    handlePlayingEvents(event);
                    break;
                case GUIState::GameOver:
                    handleGameOverEvents(event);
                    break;
            }
        }
        
        if (state == GUIState::Playing && !gameOver) {
            updatePlaying();
        }
        
        if (state == GUIState::MainMenu) {
            for (auto& btn : menuButtons) btn.update(mousePos);
        } else if (state == GUIState::PlacingShips) {
            rotateButton.update(mousePos);
            autoPlaceButton.update(mousePos);
            if (shipPlacement.allPlaced()) startGameButton.update(mousePos);
        } else {
            restartButton.update(mousePos);
        }
        
        window.clear(sf::Color::White);
        
        switch (state) {
            case GUIState::MainMenu:
                drawMenu();
                break;
            case GUIState::PlacingShips:
                drawPlacing();
                break;
            case GUIState::Playing:
                drawPlaying();
                break;
            case GUIState::GameOver:
                drawGameOver();
                break;
        }
        
        window.display();
    }
}

void GameGUI::handleMenuEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (menuButtons[0].isPressed(event, mousePos)) {
        startNewGame();
    }
    if (menuButtons[1].isPressed(event, mousePos)) {
        window.close();
    }
}

void GameGUI::handlePlacingEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::R) {
            shipPlacement.isVertical = !shipPlacement.isVertical;
            rotateButton.setText(shipPlacement.isVertical ? "Повернуть (Верт.)" : "Повернуть (Гор.)", font);
        }
    }
    
    if (rotateButton.isPressed(event, mousePos)) {
        shipPlacement.isVertical = !shipPlacement.isVertical;
        rotateButton.setText(shipPlacement.isVertical ? "Повернуть (Верт.)" : "Повернуть (Гор.)", font);
    }
    
    if (autoPlaceButton.isPressed(event, mousePos)) {
        player->getField().placeAllShipsAuto();
        shipPlacement.shipsToPlace[0] = 0;
        shipPlacement.shipsToPlace[1] = 0;
        shipPlacement.shipsToPlace[2] = 0;
        shipPlacement.shipsToPlace[3] = 0;
        statusMessage = "Корабли расставлены. Нажмите кнопку для старта.";
        
        startGameButton.setPosition(GameConfig::WINDOW_WIDTH - 250, GameConfig::WINDOW_HEIGHT - 80);
        startGameButton.setSize(200, 40);
        startGameButton.setText("В БОЙ!", font);
    }
    
    if (shipPlacement.allPlaced()) {
        if (startGameButton.contains(sf::Vector2i(GameConfig::WINDOW_WIDTH / 2.0f - 150, 200)) || 
            startGameButton.contains(sf::Vector2i(GameConfig::WINDOW_WIDTH / 2.0f, 200))) {
             startGameButton.setPosition(GameConfig::WINDOW_WIDTH - 250, GameConfig::WINDOW_HEIGHT - 80);
             startGameButton.setSize(200, 40);
             startGameButton.setText("В БОЙ!", font);
        }

        if (startGameButton.isPressed(event, mousePos)) {
            state = GUIState::Playing;
            statusMessage = "Ваш ход!";
        }
    } else {
        if (event.type == sf::Event::MouseButtonReleased && 
            event.mouseButton.button == sf::Mouse::Left) {
            
            auto coords = playerFieldRenderer->getFieldCoords(mousePos);
            if (coords.first != -1) {
                int size = shipPlacement.getNextShipSize();
                if (size > 0) {
                    if (player->getField().placeShip(coords.first, coords.second, size, shipPlacement.isVertical)) {
                        shipPlacement.shipsToPlace[4 - size]--;
                        
                        if (shipPlacement.allPlaced()) {
                            startGameButton.setPosition(GameConfig::WINDOW_WIDTH - 250, GameConfig::WINDOW_HEIGHT - 80);
                            startGameButton.setSize(200, 40);
                            startGameButton.setText("В БОЙ!", font);
                            statusMessage = "Корабли расставлены. В БОЙ!";
                        }
                    }
                }
            }
        }
    }
}

void GameGUI::handlePlayingEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    if (isPlayerTurn && !gameOver) {
        if (event.type == sf::Event::MouseButtonReleased && 
            event.mouseButton.button == sf::Mouse::Left) {
            
            auto coords = computerFieldRenderer->getFieldCoords(mousePos);
            if (coords.first != -1) {
                Field::AttackResult result = computer->getField().attack(coords.first, coords.second);
                
                if (result != Field::AttackResult::AlreadyHit && result != Field::AttackResult::Invalid) {
                    playerShots++;
                    if (result == Field::AttackResult::Hit || result == Field::AttackResult::Destroyed) {
                        playerHits++;
                        statusMessage = "Попадание! Ходите снова.";
                        if (result == Field::AttackResult::Destroyed) statusMessage = "Корабль уничтожен! Ходите снова.";
                        checkGameOver();
                    } else {
                        statusMessage = "Мимо! Ход компьютера.";
                        isPlayerTurn = false;
                    }
                }
            }
        }
    }
}

void GameGUI::handleGameOverEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (restartButton.isPressed(event, mousePos)) {
        state = GUIState::MainMenu;
        initializeMenuButtons();
    }
}

void GameGUI::updatePlaying() {
    if (!isPlayerTurn && !gameOver) {
        if (!computerThinking) {
            computerThinking = true;
            computerThinkClock.restart();
        } else {
            if (computerThinkClock.getElapsedTime().asSeconds() > 0.5f) {
                auto coords = computer->makeMove();
                Field::AttackResult result = player->getField().attack(coords.first, coords.second);
                
                computerShots++;
                
                if (result == Field::AttackResult::Hit || result == Field::AttackResult::Destroyed) {
                    computerHits++;
                    computer->onHit(coords.first, coords.second);
                    statusMessage = "Компьютер попал! Его ход.";
                    if (result == Field::AttackResult::Destroyed) {
                        computer->onDestroyed();
                        statusMessage = "Компьютер уничтожил ваш корабль!";
                    }
                    checkGameOver();
                    computerThinking = false; 
                    computerThinkClock.restart();
                } else {
                    statusMessage = "Компьютер промахнулся. Ваш ход!";
                    isPlayerTurn = true;
                    computerThinking = false;
                }
            }
        }
    }
}

void GameGUI::checkGameOver() {
    if (computer->hasLost()) {
        gameOver = true;
        winnerName = "Игрок";
        state = GUIState::GameOver;
    } else if (player->hasLost()) {
        gameOver = true;
        winnerName = "Компьютер";
        state = GUIState::GameOver;
    }
}

void GameGUI::drawMenu() {
    sf::Text title(fromUtf8("МОРСКОЙ БОЙ"), font, 50);
    title.setFillColor(sf::Color(0, 50, 100));
    title.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition((GameConfig::WINDOW_WIDTH - bounds.width) / 2, 50);
    window.draw(title);
    
    for (auto& btn : menuButtons) {
        btn.draw(window);
    }
}

void GameGUI::drawPlacing() {
    playerFieldRenderer->draw(window, player->getField());
    drawShipsToPlace();
    
    rotateButton.draw(window);
    autoPlaceButton.draw(window);
    if (shipPlacement.allPlaced()) {
        startGameButton.draw(window);
    } else {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        auto coords = playerFieldRenderer->getFieldCoords(mousePos);
        if (coords.first != -1) {
            int size = shipPlacement.getNextShipSize();
            bool canPlace = player->getField().canPlaceShip(coords.first, coords.second, size, shipPlacement.isVertical);
            playerFieldRenderer->drawShipPreview(window, coords.first, coords.second, size, shipPlacement.isVertical, canPlace);
        }
    }
    
    drawStatus();
}

void GameGUI::drawPlaying() {
    playerFieldRenderer->draw(window, player->getField());
    computerFieldRenderer->draw(window, computer->getField());
    
    drawStatus();
    drawStatistics();
}

void GameGUI::drawGameOver() {
    drawPlaying();
    
    sf::RectangleShape overlay(sf::Vector2f(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    
    sf::Text winText(fromUtf8(winnerName + " ПОБЕДИЛ!"), font, 60);
    winText.setFillColor(sf::Color::Green);
    winText.setOutlineColor(sf::Color::White);
    winText.setOutlineThickness(2);
    sf::FloatRect bounds = winText.getLocalBounds();
    winText.setPosition((GameConfig::WINDOW_WIDTH - bounds.width) / 2, GameConfig::WINDOW_HEIGHT / 2 - 50);
    window.draw(winText);
    
    restartButton.setPosition((GameConfig::WINDOW_WIDTH - 200) / 2, GameConfig::WINDOW_HEIGHT / 2 + 50);
    restartButton.draw(window);
}

void GameGUI::drawStatus() {
    sf::Text status(fromUtf8(statusMessage), font, 24);
    status.setFillColor(sf::Color::Black);
    sf::FloatRect bounds = status.getLocalBounds();
    status.setPosition((GameConfig::WINDOW_WIDTH - bounds.width) / 2, GameConfig::WINDOW_HEIGHT - 130);
    window.draw(status);
}

void GameGUI::drawShipsToPlace() {
    int startX = GameConfig::MARGIN * 2 + GameConfig::FIELD_SIZE * GameConfig::CELL_SIZE + 50;
    int startY = 100;
    
    sf::Text header(fromUtf8("Осталось расставить:"), font, 20);
    header.setFillColor(sf::Color::Black);
    header.setPosition(startX, startY);
    window.draw(header);
    
    std::string names[] = {"Линкор (4): ", "Крейсер (3): ", "Эсминец (2): ", "Катер (1): "};
    for (int i = 0; i < 4; i++) {
        sf::Text txt(fromUtf8(names[i] + std::to_string(shipPlacement.shipsToPlace[i])), font, 18);
        txt.setFillColor(sf::Color(50, 50, 50));
        txt.setPosition(startX, startY + 40 + i * 30);
        window.draw(txt);
    }
}

void GameGUI::drawStatistics() {
    sf::Text statsText;
    statsText.setFont(font);
    statsText.setCharacterSize(16);
    statsText.setFillColor(sf::Color::Black);
    
    std::string pStats = "Игрок:\nВыстрелов: " + std::to_string(playerShots) + 
                         "\nПопаданий: " + std::to_string(playerHits);
    statsText.setString(fromUtf8(pStats));
    statsText.setPosition(GameConfig::MARGIN, GameConfig::WINDOW_HEIGHT - 60);
    window.draw(statsText);
    
    std::string cStats = "Компьютер:\nВыстрелов: " + std::to_string(computerShots) + 
                         "\nПопаданий: " + std::to_string(computerHits);
    statsText.setString(fromUtf8(cStats));
    statsText.setPosition(GameConfig::WINDOW_WIDTH - 200, GameConfig::WINDOW_HEIGHT - 60);
    window.draw(statsText);
}