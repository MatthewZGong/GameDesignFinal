/**
* Author: Matthew Gong
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "EndScreen.h"
#include "Utility.h"

EndScreen::EndScreen(int l){
    font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    g_game_state.next_scene_id = End;
    lives = l;

}

EndScreen::~EndScreen()
{
    delete    g_game_state.player;
//    Mix_FreeChunk(g_game_state.jump_sfx);
//    Mix_FreeMusic(g_game_state.bgm);
}

void EndScreen::initialise()
{
    g_game_state.player = new Entity();
    g_game_state.state = 0;

}

void EndScreen::update(float delta_time)
{
    
}


void EndScreen::render(ShaderProgram *program)
{
    std::string text;
    if(lives > 0) text = "You Win!";
    else text = "You Lose";
    Utility::draw_text(program,font_texture_id, text, 0.55f, 0.0001f, glm::vec3(-4.9,1.0,0.0));
}
