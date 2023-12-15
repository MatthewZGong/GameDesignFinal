/**
* Author: Matthew Gong
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
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
//    Mix_FreeChunk(g_game_state.jump_sfx);
//    Mix_FreeMusic(g_game_state.bgm);
}

void MenuScene::initialise()
{
    g_game_state.player = new Entity();
    g_game_state.state = 1;

}

void MenuScene::update(float delta_time)
{
    
}


void MenuScene::render(ShaderProgram *program)
{
    Utility::draw_text(program,font_texture_id, "Scuffed Platformer", 0.55f, 0.0001f, glm::vec3(-4.9,1.0,0.0));
    
    Utility::draw_text(program,font_texture_id, "Press Enter To Play", 0.5f, 0.0001f, glm::vec3(-4.5,0.0,0.0));
}
