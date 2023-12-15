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
    E, E, E, E, E, E, E, E, E, E, E, E, E, E, E,
    21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

LevelB::LevelB(){
    g_number_of_enemies = 0;
    g_game_state.next_scene_id = Level2;
    g_game_state.state = 0;

}

LevelB::~LevelB()
{
//    delete    g_game_state.player;
    delete    g_game_state.map;
    delete g_game_state.player2;
    delete g_game_state.player1;
    Mix_FreeChunk(g_game_state.jump_sfx);
    Mix_FreeMusic(g_game_state.bgm);
}

void LevelB::initialise()
{
  
    g_game_state.state = 0;

    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_B_DATA, map_texture_id, 1.0f, 20, 9);
    GLuint castle_tileset_texture_id = Utility::load_texture(CASTLE_TILESET1_FILEPATH);
    GLuint entity_tileset_texture_id = Utility::load_texture(ENTITY_1_TILESET_FILEPATH);

    g_game_state.player1 = new SpawnerBase(true, 500);
    g_game_state.player1->set_position(glm::vec3(0.0f, -1.5f, 0.0f));
    g_game_state.player1->set_movement(glm::vec3(0.0f));
    g_game_state.player1->m_texture_id = castle_tileset_texture_id;
    g_game_state.player1->set_entity_type(PLAYER);

    g_game_state.player1->m_animation_cols    = 11;
    g_game_state.player1->m_animation_rows    = 27;
    g_game_state.player1->m_animation_indices = new int[1] {89};
    g_game_state.player1->m_animation_frames  = 1;
    g_game_state.player1->m_animation_index   = 0;
    g_game_state.player1->m_animation_time    = 0.0f;

//    g_game_state.player1->spawn(KNIGHT);
//    g_game_state.player1->spawn();


    
    g_game_state.player2 = new SpawnerBase(false, 100);
    g_game_state.player2->set_position(glm::vec3(13.5f, -1.5f, 0.0f));
    g_game_state.player2->set_movement(glm::vec3(0.0f));
    g_game_state.player2->m_texture_id = castle_tileset_texture_id;
    g_game_state.player2->set_entity_type(PLAYER);

    g_game_state.player2->m_animation_cols    = 11;
    g_game_state.player2->m_animation_rows    = 27;
    g_game_state.player2->m_animation_indices = new int[1] {89};
    g_game_state.player2->m_animation_frames  = 1;
    g_game_state.player2->m_animation_index   = 0;
    g_game_state.player2->m_animation_time    = 0.0f;


//    g_game_state.player2->spawn(ORC);
    g_game_state.player2->spawn(SLIME);
    g_game_state.player2->setSpawnerAi(TUTORIAL);
    // ––––– PLAYER ––––– //
    // Existing

    
    g_game_state.player = g_game_state.player1->addHero(glm::vec3(0.0f, 2.0f, 0.0f), 1, entity_tileset_texture_id);

    
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
    g_game_state.player1->update(delta_time, g_game_state.player2, g_game_state.map);
    g_game_state.player2->update(delta_time, g_game_state.player1, g_game_state.map);

    
}


void LevelB::render(ShaderProgram *program)
{
    g_game_state.map->render(program);
//    g_game_state.player->render(program);
    g_game_state.player1->render(program);
    g_game_state.player2->render(program);
    g_game_state.player1->render_attacks(program);
    g_game_state.player2->render_attacks(program);

}
