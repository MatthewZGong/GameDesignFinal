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
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

Entity::Entity()
{
    // ––––– PHYSICS ––––– //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);

    // ––––– TRANSLATION ––––– //
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
    m_dead = false;
}

Entity::~Entity()
{
    delete[] m_animation_up;
    delete[] m_animation_down;
    delete[] m_animation_left;
    delete[] m_animation_right;
    if(m_animation_hat != NULL){
        delete m_animation_hat;
    }
    if(m_animation_indices != NULL){
        delete m_animation_indices;
    }
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
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

    float vertices[] =
    {
        float(y_direction_facing)*-0.5f, -0.5f, float(y_direction_facing)*0.5f, -0.5f,  float(y_direction_facing)*0.5f, 0.5f,
        float(y_direction_facing)*-0.5f, -0.5f, float(y_direction_facing)*0.5f,  0.5f, float(y_direction_facing)*-0.5f, 0.5f

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






void Entity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map)
{
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;


    
    // ––––– ANIMATION ––––– //
    if (m_animation_indices != NULL)
    {
        //there is a better way of doing this but im lazy
        if (glm::length(m_movement) != 0 || m_animation_index != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / SECONDS_PER_FRAME;

            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }


    // ––––– GRAVITY ––––– //
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    if(m_dead) return;

    m_velocity.y += m_movement.y * m_speed;
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);



    // ––––– JUMPING ––––– //
    if (m_is_jumping)
    {

        m_is_jumping = false;

        // STEP 2: The player now acquires an upward velocity
        m_velocity.y += m_jumping_power;
    }
//    std::cout << m_collided_bottom << std::endl;
    // ––––– TRANSFORMATIONS ––––– //
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}




void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];
    

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
            if(m_entity_type == PLAYER && collidable_entity->get_entity_type() == ENEMY && !m_collided_bottom){
            }
            if(m_entity_type == END_GOAL && collidable_entity->get_entity_type() == PLAYER)
                m_dead = true;
            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
    if(m_position.y < -3.5f) m_dead = true;
}

void const Entity::check_collision_x(std::vector<Entity*>& collidable_entities)
{
    for (int i = 0; i < collidable_entities.size(); i++)
    {
        Entity* collidable_entity = collidable_entities[i];



        if (check_collision(collidable_entity))
        {

            if(m_entity_type == PLAYER && collidable_entity->get_entity_type() == ENEMY){
                m_dead = true;
            }
            if(m_entity_type == END_GOAL && collidable_entity->get_entity_type() == PLAYER)
                m_dead = true;
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}
void Entity::update(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;


    
    // ––––– ANIMATION ––––– //
    if (m_animation_indices != NULL)
    {
        //there is a better way of doing this but im lazy
        if (glm::length(m_movement) != 0 || m_animation_index != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / SECONDS_PER_FRAME;

            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }


    // ––––– GRAVITY ––––– //
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;
    
    check_collision_x(main_spawn, 1);
    check_collision_y(main_spawn, 1);

    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities);
    check_collision_x(map);
    if(m_dead) return;

    m_velocity.y += m_movement.y * m_speed;
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities);
    check_collision_y(map);
    
    

    // ––––– TRANSFORMATIONS ––––– //
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}


void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];



        if (check_collision(collidable_entity))
        {

            if(m_entity_type == PLAYER && collidable_entity->get_entity_type() == ENEMY){
                m_dead = true;
            }
            if(m_entity_type == END_GOAL && collidable_entity->get_entity_type() == PLAYER)
                m_dead = true;
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}

void const Entity::check_collision_y(std::vector<Entity*>& collidable_entities)
{
    for (int i = 0; i < collidable_entities.size(); i++)
    {
        Entity* collidable_entity = collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
            if(m_entity_type == PLAYER && collidable_entity->get_entity_type() == ENEMY && !m_collided_bottom){
            }
            if(m_entity_type == END_GOAL && collidable_entity->get_entity_type() == PLAYER)
                m_dead = true;
            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
    if(m_position.y < -3.5f) m_dead = true;
}

void Entity::render(ShaderProgram* program)
{
    if(!m_is_active) return;
    program->SetModelMatrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        if(m_animation_hat != NULL){
            glm::mat4 m_hat_matrix;

            m_hat_matrix = glm::mat4(1.0f);
            m_hat_matrix = glm::translate(m_hat_matrix, m_position+glm::vec3(0.0, 1.0, 0.0));
            program->SetModelMatrix(m_hat_matrix);
            draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_hat[m_animation_index]);
        }
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
    
    
}

bool const Entity::check_collision(Entity* other) const
{
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}


void const Entity::check_collision_y(Map* map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void Entity::kill(){
    m_dead = true;
    m_position = glm::vec3(-100,-100,0);
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}

std::pair<glm::vec3,glm::vec3> Entity::get_x_boundary(){
    glm::vec3 left = m_position + glm::vec3(-m_width/2.0f, 0.0f, 0.0f);
    glm::vec3 right = m_position + glm::vec3(m_width/2.0f, 0.0f, 0.0f);

    return {left,right};
}


void Entity::receive_attack(AttackInfo a){}
void Entity::decide_action(Entity* enemy_camp, std::vector<Entity*>& enemies){}

void Entity::render_effects(ShaderProgram *program){
};
void Entity::update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){};
