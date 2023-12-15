/**
* Author: Matthew Gong
* Assignment: Scuffed Battle Simulator
* Date due: 2023-12-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Scene.h"

class EndScreen : public Scene {
public:
    
    
    EndScreen(int lives);
    // ————— CONSTRUCTOR ————— //
    ~EndScreen();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
private:
    GLuint font_texture_id;
    int lives;
    
};


