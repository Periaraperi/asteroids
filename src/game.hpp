#pragma once

#include <memory>
#include <vector>

class Graphics;
class Input_Manager;
class Ship;
class Asteroid;
class Bullet;

class Game {
public:
    Game(Graphics& graphics, Input_Manager& input_manager);
    ~Game();
    
    void run();

private:
    void update(float dt);
    void render();

private:
    Graphics& _graphics;
    Input_Manager& _input_manager;
    
    std::unique_ptr<Ship> _ship;
    std::vector<std::unique_ptr<Asteroid>> _asteroids;
    std::vector<Bullet> _bullets;

public:
    // disable copy move ops
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;
};
