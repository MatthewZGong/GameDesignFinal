/**
* Author: Matthew Gong
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
//#include "Entity.h"
#include "Map.h"


#include "Utility.h"

#include "Scene.h"
#include "LevelB.h"
#include "LevelC.h"
#include "LevelD.h"
#include "MenuScene.h"
#include "EndScreen.h"
#include "Effects.h"




// ––––– CONSTANTS ––––– //
const int   WINDOW_WIDTH = 640,
            WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
            BG_BLUE = 0.549f,
            BG_GREEN = 0.9059f,
            BG_OPACITY = 1.0f;

const int   VIEWPORT_X = 0,
            VIEWPORT_Y = 0,
            VIEWPORT_WIDTH = WINDOW_WIDTH,
            VIEWPORT_HEIGHT = WINDOW_HEIGHT;


const float MILLISECONDS_IN_SECOND  = 1000.0;
            
const char  V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[] = "shaders/fragment_textured.glsl";




// ––––– GLOBAL VARIABLES ––––– //
const int   PLAY_ONCE   = 0,
            NEXT_CHNL   = -1; // next available channel

// ————— GLOBAL VARIABLES ————— //
Scene*  g_current_scene;

Effects* g_effects;
SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_paused = true;
int g_lives = 3;
SceneType g_current_scene_type;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_time_accumulator = 0.0f;

GLuint font_texture_id;



void switch_scenes(SceneType next){
    delete g_current_scene;
    switch(next){
        case Menu:
            g_current_scene = new MenuScene();
            break;
        case Level1:
            g_current_scene = new LevelB();
            break;
        case Level2:
            g_current_scene = new LevelC();
            break;
        case Level3:
            g_current_scene = new LevelD();
            break;
        case End:
            g_current_scene = new EndScreen(g_lives);
            break;
    }
    g_current_scene_type = next;
    g_current_scene->initialise();
        
    
}
void manage_scene()
{
    if(g_game_paused)
        return;
    
    if(g_current_scene->g_game_state.state == 1){
        g_effects->start(NEXT_LEVEL, 2.0f);
        switch_scenes(g_current_scene->g_game_state.next_scene_id);
    }
    else if(g_current_scene->g_game_state.state == 2){

        g_effects->start(DIE, 2.0f);
        g_lives -= 1;
        if(g_lives > 0)
            switch_scenes(g_current_scene_type);
        else
            switch_scenes(End);
    }

}





void initialise()
{
    // Initialising both the video AND audio subsystems
    // We did something similar when we talked about video game controllers
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Rise of the AI!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif
    // ––––– VIDEO SETUP ––––– //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.Load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.SetProjectionMatrix(g_projection_matrix);
    g_shader_program.SetViewMatrix(g_view_matrix);

    glUseProgram(g_shader_program.programID);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // ————— Game SETUP ————— //
    g_game_paused = true;
    g_lives = 3;
    g_current_scene = new MenuScene();
    g_current_scene_type = Menu;
    g_current_scene->initialise();
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    
    // ––––– GENERAL ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
}

void process_input()
{
    g_current_scene->g_game_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;
        

        

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                    g_game_paused = false;
                break;
            case SDLK_p:
                g_game_paused = !g_game_paused;
                break;
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;
            case SDLK_x:
                    if(g_current_scene->g_game_state.player2 != NULL)
                    g_current_scene->g_game_state.player->attack(g_current_scene->g_game_state.player2, g_current_scene->g_game_state.player2->get_soilders());
                break;
            case SDLK_1:
                    if(g_current_scene_type != Menu && g_current_scene_type != End)
                        g_current_scene->g_game_state.player1->spawn(KNIGHT);
                break;

            case SDLK_SPACE:
                if (!g_game_paused && g_current_scene->g_game_state.player->m_collided_bottom)
                {
                    g_current_scene->g_game_state.player->m_is_jumping = true;
                    Mix_PlayChannel(
                        NEXT_CHNL,       // using the first channel that is not currently in use...
                                    g_current_scene->g_game_state.jump_sfx,  // ...play this chunk of audio...
                        PLAY_ONCE        // ...once.
                    );
                }
                break;

            case SDLK_m:
                // Mute volume
                Mix_HaltMusic();
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if(g_game_paused)
        return;
    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene-> g_game_state.player->move_left();
    }
    else if ( key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->g_game_state.player->move_right();
    }

    // This makes sure that the player can't move faster diagonally
    if (glm::length(g_current_scene->g_game_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->g_game_state.player->set_movement(glm::normalize(g_current_scene->g_game_state.player->get_movement()));
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    if(g_game_paused){
        return;
    }

    delta_time += g_time_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        if( (g_current_scene->g_game_state.player != NULL && g_current_scene->g_game_state.player->get_dead()) || (g_current_scene->g_game_state.player1 != NULL && g_current_scene->g_game_state.player1->get_dead()))
            g_current_scene->g_game_state.state = 2;
        if( (g_current_scene->g_game_state.player2 != NULL && g_current_scene->g_game_state.player2->get_dead()))
            g_current_scene->g_game_state.state = 1;
        delta_time -= FIXED_TIMESTEP;
        g_effects->update(FIXED_TIMESTEP);

    }

    g_time_accumulator = delta_time;
    
    //player camera
    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->g_game_state.player->get_position().x, 0.0f, 0.0f));
   
}

void make_ui(){
    if(g_current_scene_type == Menu || g_current_scene_type == End) return;
//    std::cout << "GOT HERE" << std::endl;
    Utility::draw_text(&g_shader_program,font_texture_id, "Lives: "+std::to_string(g_lives), 0.55f, -0.2f, glm::vec3(g_current_scene->g_game_state.player->get_position().x+-4.9,3.0,0.0));
    Utility::draw_text(&g_shader_program,font_texture_id, "Gold:"+std::to_string(g_current_scene->g_game_state.player1->get_gold()), 0.55f, -0.2f, glm::vec3(g_current_scene->g_game_state.player->get_position().x+2.3,3.0,0.0));
}
void print_pause_screen(){
    if(g_current_scene_type == Menu || g_current_scene_type == End) return;
    Utility::draw_text(&g_shader_program,font_texture_id, "Scuffed Platformer", 0.55f, 0.0001f, glm::vec3(g_current_scene->g_game_state.player->get_position().x+-4.9,1.0,0.0));
    Utility::draw_text(&g_shader_program,font_texture_id, "Press Enter To Play", 0.5f, 0.0001f, glm::vec3(-4.5,0.0,0.0));

}
void render()

{
    
    g_shader_program.SetViewMatrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);

    g_current_scene->render(&g_shader_program);
    
    make_ui();
    if(g_game_paused)
        print_pause_screen();
    g_effects->render();


    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    
    
    SDL_Quit();
    delete g_current_scene;

}


// ––––– GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        manage_scene();
        process_input();
        render();

        if(!g_game_paused ){
            update();
        }
        
    }

    shutdown();
    return 0;
}
