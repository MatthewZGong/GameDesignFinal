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
//            std::cout << timer << std::endl;
            

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
    if(m_model_matrix != glm::translate(glm::mat4(1.0f), m_position)){
        m_model_matrix = glm::translate(m_model_matrix, m_position);
    }
    program->SetModelMatrix(m_model_matrix);

    if (m_is_active && m_animation_indices != NULL && m_animation_index < m_animation_frames)
    {
        Entity::draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);

    }
}

void AttackEntity::set_active(glm::vec3 position, float move_direction, std::vector<Entity*> attacking){
    if(timer != 0) return;
    m_is_active = true;
    m_animation_index = 0;

    m_position = position+glm::vec3(1.0*move_direction,0,0);
//    std::cout << position.x << " " << m_position.x << std::endl;
    timer = cooldown;
    for(Entity* e: attacking){
        e->receive_attack(attackInfo);
    }
}

Unit::~Unit(){
    delete death_animation;
}

Unit::Unit(){
    worth = 10;
    m_dead = false;
    death_animation = new AnimationInfo;
    death_animation->index = 0;
    death_animation->animation_rows = 9;
    death_animation->animation_cols = 28;
    death_animation->animation_indices = {62, 63, 64, 65};
    death_animation->texture = Utility::load_texture(ENTITY_TILESET2_FILEPATH);
    m_position = glm::vec3(0.0f, 300.0f, 0.0f);
    m_model_matrix = glm::translate(glm::mat4(1.0f), m_position);
    
    
}

void Unit::update(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){
    
    // –––––Death ANIMATION ––––– //
    if(!m_is_active) return;
    // ––––– ANIMATION ––––– //
    if (health <= 0)
    {
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;

        if (m_animation_time >= frames_per_second)
        {
            death_animation->index++;
        }
        if(death_animation->index >= death_animation->animation_indices.size()){
            main_spawn->addGold(worth);
            m_is_active = false;
            m_dead = true;
        }
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
    
        return;
    }
    
    Entity::update(delta_time, main_spawn, collidable_entities, map);
}

void Unit::render(ShaderProgram* program){
    if(!m_is_active) return;
    if(health <= 0){
        //this is scuffed i was being lazy

        program->SetModelMatrix(m_model_matrix);
        std::swap(m_animation_rows, death_animation->animation_rows);
        std::swap(m_animation_cols, death_animation->animation_cols);
        draw_sprite_from_texture_atlas(program, death_animation->texture, death_animation->animation_indices[death_animation->index]);
        std::swap(m_animation_rows, death_animation->animation_rows);
        std::swap(m_animation_cols, death_animation->animation_cols);
        return;
    }
    
    Entity::render(program);
    
//    swap(m_animation_rows, death_animation->animation_rows);
//    swap(m_animation_cols, death_animation->animation_cols);
//    swap(m_texture_id, death_animation->texture);
}

BasicMelee::BasicMelee(int cd, float direction): attack_entity(cd, direction){
    
}
BasicMelee::~BasicMelee(){
    
}




void BasicMelee::receive_attack(AttackInfo a)
{
    health -= a.dmg;
    m_position.x += -(move_direction)*a.knock_back;
    if(health < 0){
        health = 0;
    }
    
}

void BasicMelee::decide_action(Entity* enemy_camp, std::vector<Entity*>&
                           enemies)
{
    if(!m_is_active || health <= 0) return;
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
void BasicMelee::render_effects(ShaderProgram *program)
{
//    std::cout << "i am rendering" << std::endl;
    
    attack_entity.render(program);
}

void BasicMelee::update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){
    attack_entity.update(delta_time, main_spawn, collidable_entities, map);
};



SpawnerBase::SpawnerBase(bool forward, int h)
{
    y_direction_facing = forward ? 1.0f : -1.0f;
    unit_texture_id = Utility::load_texture(ENTITY_TILESET1_FILEPATH);
    m_width = 1.5;
    health = h;
    gold = 0;
    
}
SpawnerBase::~SpawnerBase(){
    for(Entity* soldier: soldiers){
        delete soldier;
    }
}

void SpawnerBase::ai_action(){
    if(aitype == FLYING){
        if(frames_alive%100 == 0){
            spawn(BAT);
        }
        if(frames_alive%1000 == 0){
            spawn(ORC);
        }

    }else if(aitype == EVERYTHING){
        int r = -1;
        if(frames_alive%222 == 0){
            int r = (rand()%3);
        }
        if(r == 0 || frames_alive%100 == 0){
            spawn(SLIME);
        }
        if(r == 1 || frames_alive%400 == 0){
            spawn(BAT);
        }
        if(r == 2 || frames_alive%700 == 0){
            spawn(ORC);
        }

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
    std::vector<Entity*> temp;
    Unit::update(delta_time, EnemyCamp, temp, map);
    ai_action();
    frames_alive++;
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
    Unit::render(program);
    for(int i =0; i < soldiers.size(); i++){
        soldiers[i]->render(program);
    }
}

void SpawnerBase::spawn(SoilderType st){
    if(st == KNIGHT){
        if(gold < 10) return;
        gold -= 10;
        soldiers.push_back(new Knight(m_position, y_direction_facing, unit_texture_id));
    }
    else if(st == WIZARD){
        if(gold < 0) return;
        gold -= 0;
        soldiers.push_back(new Wizard(m_position, y_direction_facing, unit_texture_id));
    }
    else if(st == ORC)
        soldiers.push_back(new Orc(m_position, y_direction_facing, unit_texture_id));
    else if(st == SLIME)
        soldiers.push_back(new Slime(m_position, y_direction_facing, unit_texture_id));
    else if(st == BAT)
        soldiers.push_back(new Bat(m_position, y_direction_facing, unit_texture_id));
}

void SpawnerBase::receive_attack(AttackInfo a){
    health -= a.dmg;
    if(health < 0){
//        m_is_active = false;
        health = 0;
    }
}

void SpawnerBase::render_attacks(ShaderProgram* program){
    for(int i = 0; i < soldiers.size(); i++){
        soldiers[i]->render_effects(program);
    }
}

Hero* SpawnerBase::addHero(glm::vec3 position, float direction, GLuint texture_id){
    Hero* h = new Hero(position, direction, texture_id);
    soldiers.push_back(h);
    return h;
}


Hero::~Hero(){
    m_dead = false;
}

Hero::Hero(): attack_entity(1, -1.0) {
    
}

Hero::Hero(glm::vec3 position, float direction, GLuint texture_id): attack_entity(4, direction)
{
    
    

    
    health = 200;
    attack_range = 1.5f;
    m_texture_id = texture_id;
    move_direction = float(direction);
    y_direction_facing = float(direction);
    Entity::set_position(position);
    Entity::set_movement(glm::vec3(0.0f));
    Entity::set_speed(1.5f);
    Entity::set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_animation_cols = 32;
    m_animation_rows = 32;
    m_animation_indices = new int[3] {104,110, 111};
    m_animation_hat = new int[3] {72, 78, 79};
    m_animation_frames = 3;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    m_width = 0.6f;
    m_height = 0.6f;
    set_jumping_power(6.0f);
    
    attack_entity.attackInfo = {75, 1.0};
}

void Hero::Hero::receive_attack(AttackInfo a)
{
    health -= a.dmg;
//    m_position.x += -(move_direction)*a.knock_back;
    if(health < 0){
        health = 0;
    }
    
}

void Hero::render_effects(ShaderProgram *program)
{    
    attack_entity.render(program);
}

void Hero::update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){
    attack_entity.update(delta_time, main_spawn, collidable_entities, map);
};


void Hero::attack(Entity* enemy_camp, std::vector<Entity*>&
                  enemies){
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
    

    if(!attack_entity.get_active()){
        attack_entity.set_y_facing_direction(y_direction_facing);
        if(distances < attack_range)
            attack_entity.set_active(m_position,y_direction_facing,  {closest});
        else
            attack_entity.set_active(m_position,y_direction_facing,  {});

    }

}


Knight::Knight(glm::vec3 position, float direction, GLuint texture_id): BasicMelee(6, direction)
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


Orc::Orc(glm::vec3 position, float direction, GLuint texture_id): BasicMelee(9, direction)
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
    m_animation_indices = new int[3] {343,349, 350};
//    m_animation_hat = new int[3] {136, 142, 143};
    m_animation_frames = 3;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    m_width = 0.6f;
    m_height = 0.6f;
    attack_entity.attackInfo = {45, 1.0};
}

Slime::Slime(glm::vec3 position, float direction, GLuint texture_id): BasicMelee(4, direction)
{
    
    attack_range = 0.8f;
    worth = 2;
    health = 20;
    m_texture_id = texture_id;
    move_direction = float(direction);
    y_direction_facing = float(direction);
    Entity::set_position(position);
    Entity::set_movement(glm::vec3(0.0f));
    Entity::set_speed(2.0f);
    Entity::set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_animation_cols = 32;
    m_animation_rows = 32;
    m_animation_indices = new int[4] {251,252,253, 254};
    m_animation_frames = 4;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    m_width = 0.6f;
    m_height = 0.6f;
    attack_entity.attackInfo = {20, 0.0};
}


Bat::Bat(glm::vec3 position, float direction, GLuint texture_id): BasicMelee(4, direction)
{
    
    attack_range = 0.8f;
    worth = 5;
    health = 20;
    m_texture_id = texture_id;
    move_direction = float(direction);
    y_direction_facing = float(direction);
    position.y += 1.5f;
    Entity::set_position(position);
    Entity::set_movement(glm::vec3(0.0f));
    Entity::set_speed(2.0f);
    Entity::set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_animation_cols = 32;
    m_animation_rows = 32;
    m_animation_indices = new int[3] {120,121,122};
    m_animation_hat = new int[3] {88,89,90};
    m_animation_frames = 3;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    m_width = 0.8f;
    m_height = 0.8f;
    attack_entity.attackInfo = {20, 0.0};
}


void Bat::decide_action(Entity* enemy_camp, std::vector<Entity*>&
                                    enemies)
{
    if(!m_is_active || health <= 0) return;
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
    if( m_position.y > enemy_camp->get_position().y+0.75 && glm::distance(m_position, enemy_camp->get_position())  < 3.0 ){
        m_movement.y = -0.5f;
    }
}


Wizard::Wizard(glm::vec3 position, float direction, GLuint texture_id){
    attack_range = 3.0f;
    worth = 30;
    health = 20;
    m_texture_id = texture_id;
    move_direction = float(direction);
    y_direction_facing = float(direction);
    Entity::set_position(position);
    Entity::set_movement(glm::vec3(0.0f));
    Entity::set_speed(0.8f);
    Entity::set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_animation_cols = 32;
    m_animation_rows = 32;
    m_animation_indices = new int[4] {360,361,365,366};
    m_animation_hat = new int[4] {328,329,333,334};
    m_animation_frames = 4;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    m_width = 0.8f;
    m_height = 0.8f;
    attack_entity = new FireBall(200,direction);
    attack_entity->attackInfo = {100, 0.3};
}
Wizard::~Wizard(){
    delete attack_entity;
}




void Wizard::receive_attack(AttackInfo a)
{
    health -= a.dmg;
    m_position.x += -(move_direction)*a.knock_back;
    if(health < 0){
        health = 0;
    }
    
}

void Wizard::decide_action(Entity* enemy_camp, std::vector<Entity*>&
                           enemies)
{
    if(!m_is_active || health <= 0) return;
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
        if(!attack_entity->get_active()){
            attack_entity->set_active(m_position,move_direction,  {closest});

        }
    }else{
        m_movement.x = 1.0*move_direction;
    }
    
}
void Wizard::render_effects(ShaderProgram *program)
{
//    std::cout << "i am rendering" << std::endl;
    
    attack_entity->render(program);
}

void Wizard::update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){
    attack_entity->update(delta_time, main_spawn, collidable_entities, map);
};

FireBall::~FireBall(){
    delete death_animation;
}
FireBall::FireBall(int cd, float direction): AttackEntity(cd, direction){
    attackInfo = {100, 0.2};
    cooldown = cd;
    timer = 0;
    m_texture_id = Utility::load_texture(FIRE_TILESET1_FILEPATH);
    m_animation_cols    = 36;
    m_animation_rows    = 13;
    if(m_animation_indices != NULL)
        delete m_animation_indices;
    m_animation_indices = new int[5] {24,25,26,27,28};
    m_animation_frames  = 3;
    m_animation_index   = 0;
    m_animation_time    = 0.0f;
    m_is_active = false;
    y_direction_facing = direction;
    
    m_speed = 5.0;
    death_animation = new AnimationInfo;
    death_animation->index = 0;
    death_animation->animation_rows = 13;
    death_animation->animation_cols = 36;
    death_animation->animation_indices = {66, 67, 68, 69, 70, 71};
    death_animation->texture = m_texture_id;
    explode = false;
    
    
}
void FireBall::update(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map){
//    std::cout << timer << std::endl;
    // ––––– ANIMATION ––––– //
//    std::cout  << m_movement.x << " " << m_movement.y << std::endl;
//    if(!explode)
//        m_movement.x = y_direction_facing*1.0;
    bool hit = check_collision(main_spawn);
    for(Entity* e: collidable_entities){
        hit = hit || check_collision(e);
    }
    if(hit && !explode || frames_alive > 1000 ){
        for(Entity* e: collidable_entities){
            if(check_collision(e)){
                e->receive_attack(attackInfo);
            }
        }
        if (check_collision(main_spawn)){
            main_spawn->receive_attack(attackInfo);
        }
        explode = true;
    }
    
    if (explode)
    {
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;

        if (m_animation_time >= frames_per_second)
        {
            death_animation->index++;
            timer--;
            timer = fmax(0, timer);
        }
        if(death_animation->index >= death_animation->animation_indices.size()){
            m_is_active = false;
        }
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
    
        return;
    }

    
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
                frames_alive++;
                m_animation_time = 0.0f;
                m_animation_index++;
                timer--;
                timer = fmax(0, timer);
                if (m_animation_index >= m_animation_frames)
                {
                    
                    m_animation_index = 0;
                }
            }
        }
    }
    m_velocity.x = m_movement.x * m_speed;
    m_velocity.y = m_movement.y * m_speed;
    m_position.x += m_velocity.x * delta_time;
    m_position.y += m_velocity.y * delta_time;
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}
void FireBall::render(ShaderProgram* program){
    if(!m_is_active) return;
    if(explode){
        program->SetModelMatrix(m_model_matrix);
        std::swap(m_animation_rows, death_animation->animation_rows);
        std::swap(m_animation_cols, death_animation->animation_cols);
        draw_sprite_from_texture_atlas(program, death_animation->texture, death_animation->animation_indices[death_animation->index]);
        std::swap(m_animation_rows, death_animation->animation_rows);
        std::swap(m_animation_cols, death_animation->animation_cols);
        return;
    }
    Entity::render(program);
}

void FireBall::set_active(glm::vec3 position, float move_direction, std::vector<Entity*> attacking){
    if(timer != 0) return;
    y_direction_facing = move_direction;
    m_is_active = true;
    explode = false;
    m_animation_index = 0;
    m_position = position;
    if(attacking.size() > 0 && attacking[0] != NULL){
        glm::vec3 direction = attacking[0]->get_position()-position;
        direction = glm::normalize(direction);
        m_movement = direction;
//        std::cout  << "Start: " << m_movement.x << " " << m_movement.y << std::endl;
        y_direction_facing = m_movement.x == 1 ? -1 : 1;
    }else{
        m_movement.x = move_direction*1.0;
    }
    death_animation->index = 0;
    timer = cooldown;
    frames_alive = 0;
//    for(Entity* e: attacking){
//        e->receive_attack(attackInfo);
//    }

}
