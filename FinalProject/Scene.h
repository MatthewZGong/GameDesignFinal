/**
* Author: Matthew Gong
* Assignment: Scuffed Battle Simulator
* Date due: 2023-12-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
//#include "Entity.h"
#include "Map.h"
#include "SpawnerBase.h"

enum SceneType { Menu, Level1, Level2, Level3, End};


const char  SPRITE_TILESET_FILEPATH[] = "resources/tilemap-characters_packed.png",
            MAP_TILESET_FILEPATH[]  = "resources/tilemap_packed.png",
            FONTSHEET_FILEPATH[] = "resources/font1.png";
const char  BGM_FILEPATH[]          = "resources/adventure.mp3",
            BOUNCING_SFX_FILEPATH[] = "resources/jump.wav";

const char ENTITY_1_TILESET_FILEPATH[] = "resources/entity_tiles.png";

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map *map;
    Hero *player;
    
    SpawnerBase* player1;
    SpawnerBase* player2;
    
    // ————— AUDIO ————— //
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    
    bool m_collided_bottom;
    
    // ————— POINTERS TO OTHER SCENES ————— //
    int state; 
    SceneType next_scene_id;
    
};

class Scene {
public:
    // ————— ATTRIBUTES ————— //
    int g_number_of_enemies = 1;
    
    GameState g_game_state;
    
    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    // ————— GETTERS ————— //
    GameState const get_state()             const { return g_game_state;             }
    int       const get_number_of_enemies() const { return g_number_of_enemies; }
};


void createFlyEnemy(GLuint& sprite_tileset_texture_id, Entity* enemy, glm::vec3 position);
void createBasicEnemy(GLuint& sprite_tileset_texture_id, Entity* enemy, glm::vec3 position);
void createRamEnemy(GLuint& sprite_tileset_texture_id, Entity* enemy, glm::vec3 position);
void createEndGoal(GLuint& sprite_tileset_texture_id, Entity* goal, glm::vec3 position);

