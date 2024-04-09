#pragma once

class Graphics;
class Input_Manager;

class Exp {
public:

    Exp(Graphics& graphics, Input_Manager& input_manager);
    ~Exp();
    
    void run();

private:
    void update(float dt);
    void render();

private:
    bool _running;
    Graphics& _graphics;
    Input_Manager& _input_manager;

public:
    // disable copy move ops
    Exp(const Exp&) = delete;
    Exp& operator=(const Exp&) = delete;
    Exp(Exp&&) = delete;
    Exp& operator=(Exp&&) = delete;
};
