#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "SpawnerBase.h"
#include "Utility.h"


AttackEntity::AttackEntity(int cd, float direction){
    attackInfo = {25, 0.4};
    cooldown = cd;
    timer = 0;
    m_texture_id = Utility::load_texture(ENTITY_TILESET2_FILEPATH);
    m_animation_cols    = 28;
    m_animation_rows    = 9;
    if(m_animation_indices != NULL)
        delete m_animation_indices;
    m_animation_indices = new int[3] {96,97,98};
    m_animation_frames  = 3;
    m_animation_index   = 0;
    m_animation_time    = 0.0f;
    m_is_active = false;
    y_direction_facing = direction;
    
}
void AttackEntity::update(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){

    // ––––– ANIMATION ––––– //
    if (m_animation_indices != NULL)
    {
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;

        if (m_animation_time >= frames_per_second)
        {
            m_animation_time = 0.0f;
            m_animation_index++;
            timer--;
            timer = fmax(0, timer);
            

            if (m_animation_index >= m_animation_frames)
            {
                m_animation_index = 0;
                m_is_active = false;
            }
        }
    }


    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}
void AttackEntity::render(ShaderProgram* program){
    
//    m_model_matrix = glm::translate(m_model_matrix, m_position);
    program->SetModelMatrix(m_model_matrix);

    if (m_is_active && m_animation_indices != NULL && m_animation_index < m_animation_frames)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);

    }
}

void AttackEntity::set_active(glm::vec3 position, float move_direction, std::vector<Entity*> attacking){
    if(timer != 0) return;
    m_is_active = true;
    m_animation_index = 0;
    m_position = position+glm::vec3(1.0*move_direction,0,0);
    timer = cooldown;
    for(Entity* e: attacking){
        e->receive_attack(attackInfo);
    }
}

Unit::~Unit(){
    
}



Knight::~Knight(){
    
}


Knight::Knight(glm::vec3 position, float direction, GLuint texture_id): attack_entity(6, direction)
{
    
    attack_range = 0.8f;

    health = 100;
    m_texture_id = texture_id;
    move_direction = float(direction);
    y_direction_facing = float(direction);
    Entity::set_position(position);
    Entity::set_movement(glm::vec3(0.0f));
    Entity::set_speed(1.0f);
    Entity::set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_animation_cols = 32;
    m_animation_rows = 32;
    m_animation_indices = new int[3] {168,174, 175};
    m_animation_hat = new int[3] {136, 142, 143};
    m_animation_frames = 3;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    m_width = 0.6f;
    m_height = 0.6f;
    

}

void Knight::Knight::receive_attack(AttackInfo a)
{
    health -= a.dmg;
    m_position.x += -(move_direction)*a.knock_back;
    if(health < 0){
        health = 0;
        m_is_active = false;
    }
}

void Knight::decide_action(Entity* enemy_camp, std::vector<Entity*>&
                           enemies)
{
    if(!m_is_active) return;
    m_movement = glm::vec3(0.0);
    Entity* closest = NULL;
    float distances = 1e9;
    if(enemy_camp != NULL && enemy_camp->get_active())
    {
        closest = enemy_camp;
        auto [left,right] =enemy_camp->get_x_boundary();
        distances = glm::distance(m_position, left);
        distances = fmin(distances, glm::distance(m_position, right));
    }
    for(Entity* e: enemies)
    {
        if(e->get_active())
        {

            auto [left,right] = e->get_x_boundary();
            float cur_dist = glm::distance(m_position, left);
            cur_dist =  fmin(cur_dist, glm::distance(m_position, right));
            if(cur_dist < distances){
                distances = cur_dist;
                closest = e;
            }
        }
    }
//    std::cout << distances << std::endl;
    if(distances < attack_range){
        if(!attack_entity.get_active()){
            attack_entity.set_active(m_position,move_direction,  {closest});

        }
    }else{
        m_movement.x = 1.0*move_direction;
    }
    
}
void Knight::render_effects(ShaderProgram *program)
{
//    std::cout << "i am rendering" << std::endl;
    
    attack_entity.render(program);
}

void Knight::update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){
    attack_entity.update(delta_time, main_spawn, collidable_entities, map);
};



SpawnerBase::SpawnerBase(bool forward, int h)
{
    y_direction_facing = forward ? 1.0f : -1.0f;
    unit_texture_id = Utility::load_texture(ENTITY_TILESET1_FILEPATH);
    m_width = 1.5;
    health = h;
    
}
SpawnerBase::~SpawnerBase(){
    for(Entity* soldier: soldiers){
        delete soldier;
    }
}


void SpawnerBase::update(float delta_time, SpawnerBase* EnemyCamp, Map* map){
    std::vector<Entity*>& enemies = EnemyCamp->get_soilders();

    //this is scuffed but im lazy
    for(int i = 0; i < soldiers.size(); i++){
        soldiers[i]->decide_action(EnemyCamp, enemies);
    }
    for(int i =0; i < soldiers.size(); i++){
        soldiers[i]->update(delta_time, EnemyCamp, enemies, map);
        soldiers[i]->update_effects(delta_time, EnemyCamp, enemies, map);
    }
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}
void SpawnerBase::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

//    float vertices[] =
//    {
//        floa1.0, -1.0, -1.0, -1.0,  -1.0, 1.0,
//        1.0, -1.0, -1.0,  1.0, 1.0, 1.0
//
//    };
    float vertices[] =
    {
        float(y_direction_facing)*-1.0f, -1.0f, float(y_direction_facing)*1.0f, -1.0f,  float(y_direction_facing)*1.0f, 1.0f,
        float(y_direction_facing)*-1.0f, -1.0f, float(y_direction_facing)*1.0f,  1.0f, float(y_direction_facing)*-1.0f, 1.0f

    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    

    
}

void SpawnerBase::render(ShaderProgram* program){
    Entity::render(program);
    for(int i =0; i < soldiers.size(); i++){
        soldiers[i]->render(program);
    }
}

void SpawnerBase::spawn(){
    soldiers.push_back(new Knight(m_position, y_direction_facing, unit_texture_id));
}

void SpawnerBase::receive_attack(AttackInfo a){
    health -= a.dmg;
    if(health < 0){
        m_is_active = false;
        health = 0;
    }
}

void SpawnerBase::render_attacks(ShaderProgram* program){
    for(int i = 0; i < soldiers.size(); i++){
        soldiers[i]->render_effects(program);
    }
}

