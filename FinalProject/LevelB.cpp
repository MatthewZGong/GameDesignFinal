/**
* Author: Matthew Gong
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelB.h"
#include "Utility.h"


#define LEVEL_WIDTH 15
#define LEVEL_HEIGHT 5
const int E = -1;

int LEVEL_B_DATA[] =
{
    E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E, E, 1, 2, 2, 2, 2, 3,
    21, 22, 22, 22, 22, 22, 22, 23, E, E, E, E, E, E, E,
    4, 4, 4, 4, 4, 4, 4, 4, E, E, E, E, E, E, E
};

LevelB::LevelB(){
    g_number_of_enemies = 0;
    g_game_state.next_scene_id = Level2;
    g_game_state.state = 0;

}

LevelB::~LevelB()
{
    delete [] g_game_state.enemies;
    delete    g_game_state.player;
    delete    g_game_state.map;
    Mix_FreeChunk(g_game_state.jump_sfx);
    Mix_FreeMusic(g_game_state.bgm);
}

void LevelB::initialise()
{
  
    g_game_state.state = 0;

    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_B_DATA, map_texture_id, 1.0f, 20, 9);
    GLuint sprite_tileset_texture_id = Utility::load_texture(SPRITE_TILESET_FILEPATH);
    GLuint entity_tileset_texture_id = Utility::load_texture(ENTITY_1_TILESET_FILEPATH);

    g_game_state.player1 = new SpawnerBase(true, 500);
    g_game_state.player1->set_position(glm::vec3(0.0f, -1.5f, 0.0f));
    g_game_state.player1->set_movement(glm::vec3(0.0f));
    g_game_state.player1->m_texture_id = entity_tileset_texture_id;
    g_game_state.player1->set_entity_type(PLAYER);

    g_game_state.player1->m_animation_cols    = 32;
    g_game_state.player1->m_animation_rows    = 32;
    g_game_state.player1->m_animation_indices = new int[3] {168,174, 175};
    g_game_state.player1->m_animation_frames  = 3;
    g_game_state.player1->m_animation_index   = 0;
    g_game_state.player1->m_animation_time    = 0.0f;

    g_game_state.player1->spawn();

    
    g_game_state.player2 = new SpawnerBase(false, 100);
    g_game_state.player2->set_position(glm::vec3(5.0f, -1.5f, 0.0f));
    g_game_state.player2->set_movement(glm::vec3(0.0f));
    g_game_state.player2->m_texture_id = entity_tileset_texture_id;
    g_game_state.player2->set_entity_type(PLAYER);

    g_game_state.player2->m_animation_cols    = 32;
    g_game_state.player2->m_animation_rows    = 32;
    g_game_state.player2->m_animation_indices = new int[3] {168,174, 175};
    g_game_state.player2->m_animation_frames  = 3;
    g_game_state.player2->m_animation_index   = 0;
    g_game_state.player2->m_animation_time    = 0.0f;


    g_game_state.player2->spawn();

    // ––––– PLAYER ––––– //
    // Existing

    
    g_game_state.player = new Entity();
    g_game_state.player->set_position(glm::vec3(0.0f, 2.0f, 0.0f));
    g_game_state.player->set_movement(glm::vec3(0.0f));
    g_game_state.player->set_speed(1.5f);
    g_game_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    g_game_state.player->m_texture_id = entity_tileset_texture_id;
    g_game_state.player->set_entity_type(PLAYER);

    g_game_state.player->m_animation_cols    = 32;
    g_game_state.player->m_animation_rows    = 32;
    g_game_state.player->m_animation_indices = new int[3] {168,174, 175};
    g_game_state.player->m_animation_hat = new int[3] {136, 142, 143};
    g_game_state.player->m_animation_frames  = 3;
    g_game_state.player->m_animation_index   = 0;
    g_game_state.player->m_animation_time    = 0.0f;


    g_game_state.player->set_height(0.9f);
    g_game_state.player->set_width(0.9f);

    // Jumping
    g_game_state.player->set_jumping_power(6.0f);

    // ––––– ENEMY––––– //

    
    // ––––– AUDIO STUFF ––––– //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);

    g_game_state.jump_sfx = Mix_LoadWAV(BOUNCING_SFX_FILEPATH);

    
    // ––––– GENERAL ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void LevelB::update(float delta_time)
{
    g_game_state.player->update(delta_time, g_game_state.enemies, g_number_of_enemies, g_game_state.map);
    g_game_state.player1->update(delta_time, g_game_state.player2, g_game_state.map);
    g_game_state.player2->update(delta_time, g_game_state.player1, g_game_state.map);
    for (int i = 0; i < g_number_of_enemies; i++)
    {
        g_game_state.enemies[i].update(delta_time, 0, 0, g_game_state.map);
    }
    
}


void LevelB::render(ShaderProgram *program)
{
    g_game_state.map->render(program);
    g_game_state.player->render(program);
    g_game_state.player1->render(program);
    g_game_state.player2->render(program);
    g_game_state.player1->render_attacks(program);
    g_game_state.player2->render_attacks(program);

}
