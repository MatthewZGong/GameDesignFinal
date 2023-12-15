/**
* Author: Matthew Gong
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Map.h"
#include "Entity.h"
#include "vector"
#include <SDL.h>
#include <SDL_opengl.h>

const char ENTITY_TILESET1_FILEPATH[] = "resources/entity_tiles.png";

const char ENTITY_TILESET2_FILEPATH[] = "resources/entity_tiles_2.png";

class AttackEntity: public Entity{
private:
    int cooldown;
    int timer;
public:
    AttackEntity(int cd, float direction);
    virtual void set_active(glm::vec3 position, float move_direction, std::vector<Entity*> entities);
    void render(ShaderProgram* program);
    void update(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map);

    
};



class Unit: public Entity {
    
protected:
    int health;
    float move_direction;
    float attack_range;

public:
//    Unit();
    virtual ~Unit();


};

class Knight: public Unit {
    private:
        AttackEntity attack_entity;
    
    public:
    Knight(glm::vec3 position, float direction, GLuint texture_id);
    virtual ~Knight();
    void decide_action(Entity* enemy_camp,std::vector<Entity*>& enemies);
    void receive_attack(AttackInfo a);
    void render_effects(ShaderProgram* program);
    void update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map);
    
    
};

class SpawnerBase: public Unit
{
    private:
        std::vector<Entity*> soldiers;
        GLuint unit_texture_id;
        int gold;
        int gold_generation;
    public:
        SpawnerBase(bool forward, int h);
        virtual ~SpawnerBase();
        void update(float delta_time, SpawnerBase* EnemyCamp, Map* map);
        void render(ShaderProgram* program);
        void render_attacks(ShaderProgram* program);
        void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
        void spawn();
        void receive_attack(AttackInfo a);
        std::vector<Entity*>& get_soilders() {return soldiers;};
};

