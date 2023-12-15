/**
* Author: Matthew Gong
* Assignment: Scuffed Battle Simulator
* Date due: 2023-12-15, 11:59pm
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

const char FIRE_TILESET1_FILEPATH[] = "resources/FireEffect.png";

const char CASTLE_TILESET1_FILEPATH[] = "resources/CastleTile.png";

enum SoilderType { KNIGHT, ORC, SLIME, BAT, WIZARD, CHEAT };
enum SpawnerAI { NO_AI, FLYING, EVERYTHING, TUTORIAL };


class AttackEntity: public Entity{
protected:
    int cooldown;
    int timer;
public:
    AttackEntity(int cd, float direction);
    virtual void set_active(glm::vec3 position, float move_direction, std::vector<Entity*> entities);
    void render(ShaderProgram* program);
    void update(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map);
};

class FireBall: public AttackEntity{
private:
    bool explode;
public:
    FireBall(int cd, float direction);
    ~FireBall();
    virtual void set_active(glm::vec3 position, float move_direction, std::vector<Entity*> entities);
    void render(ShaderProgram* program);
    void update(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map);
};



class Unit: public Entity {
    
protected:
    int health;
    float move_direction;
    float attack_range;
    int worth;


public:
    Unit();
    virtual ~Unit();
    void update(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map);
    void render(ShaderProgram* program);
    int get_health(){return health;};


};

class BasicMelee: public Unit {
    protected:
        AttackEntity attack_entity;
    
    public:
    BasicMelee(int cd,float direction);
    virtual ~BasicMelee();
    void decide_action(Entity* enemy_camp,std::vector<Entity*>& enemies);
    void receive_attack(AttackInfo a);
    void render_effects(ShaderProgram* program);
    void update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map);
};

class Knight: public BasicMelee {

    
    public:
        Knight(glm::vec3 position, float direction, GLuint texture_id);

};
class Orc: public BasicMelee {

    
    public:
        Orc(glm::vec3 position, float direction, GLuint texture_id);

};

class Slime: public BasicMelee {

    
    public:
        Slime(glm::vec3 position, float direction, GLuint texture_id);

};

class Bat: public BasicMelee {

    
    public:
        Bat(glm::vec3 position, float direction, GLuint texture_id);
        void decide_action(Entity* enemy_camp,std::vector<Entity*>& enemies);


};


class Wizard: public Unit {
    protected:
        AttackEntity* attack_entity;

    public:
    Wizard(glm::vec3 position, float direction, GLuint texture_id);
    virtual ~Wizard();
    void decide_action(Entity* enemy_camp,std::vector<Entity*>& enemies);
    void receive_attack(AttackInfo a);
    void render_effects(ShaderProgram* program);
    void update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map);
};

class Hero: public Unit {
    private:
        AttackEntity attack_entity;
    
    public:
    Hero();
    Hero(glm::vec3 position, float direction, GLuint texture_id);
    virtual ~Hero();
    void attack(Entity* enemy_camp, std::vector<Entity*>&
                enemies);
    void receive_attack(AttackInfo a);
    void render_effects(ShaderProgram* program);
    void update_effects(float delta_time, Entity* main_spawn, std::vector<Entity*>& collidable_entities, Map* map);
};


class SpawnerBase: public Unit
{
    private:
        std::vector<Entity*> soldiers;
        GLuint unit_texture_id;
        SpawnerAI aitype = NO_AI;
        void ai_action();

    public:
        SpawnerBase(bool forward, int h);
        void setSpawnerAi(SpawnerAI t){ aitype = t; };
        virtual ~SpawnerBase();
        void update(float delta_time, SpawnerBase* EnemyCamp, Map* map);
        void render(ShaderProgram* program);
        void render_attacks(ShaderProgram* program);
        
        void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
        void spawn(SoilderType st);
        void receive_attack(AttackInfo a);
        Hero* addHero(glm::vec3 position, float direction, GLuint texture_id);
        std::vector<Entity*>& get_soilders() {return soldiers;};
        
};

