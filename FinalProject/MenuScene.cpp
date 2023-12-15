/**
* Author: Matthew Gong
* Assignment: Scuffed Battle Simulator
* Date due: 2023-12-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "MenuScene.h"
#include "Utility.h"

MenuScene::MenuScene(){
    font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    g_game_state.next_scene_id = Level1;

}

MenuScene::~MenuScene()
{
    delete    g_game_state.player;
    delete g_game_state.player1;
    delete g_game_state.player2;
}

void MenuScene::initialise()
{
    g_game_state.player = new Hero();
    g_game_state.player1 = new SpawnerBase(true, 1);
    g_game_state.player2 = new SpawnerBase(false, 1);
    g_game_state.state = 1;

}

void MenuScene::update(float delta_time)
{
    
}


void MenuScene::render(ShaderProgram *program)
{
    Utility::draw_text(program,font_texture_id, "Scuffed Battle Simulator", 0.55f, -0.15f, glm::vec3(-4.8,1.0,0.0));
    
    Utility::draw_text(program,font_texture_id, "Press Enter To Play", 0.5f, 0.0001f, glm::vec3(-4.5,0.0,0.0));
}
