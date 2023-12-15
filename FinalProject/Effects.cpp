/**
* Author: Matthew Gong
* Assignment: Scuffed Battle Simulator
* Date due: 2023-12-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Effects.h"

Effects::Effects(glm::mat4 projection_matrix, glm::mat4 view_matrix)
{
    // Non textured Shader
    m_program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    m_program.SetProjectionMatrix(projection_matrix);
    m_program.SetViewMatrix(view_matrix);
    
    m_current_effect = NONE;
    m_alpha = 1.0f;
    m_effect_speed = 1.0;
    m_size = 10.0f;
    
    m_view_offset = glm::vec3(0.0f);
}

void Effects::draw_overlay()
{
    glUseProgram(this->m_program.programID);

    float vertices[] =
    {
        -0.5, -0.5,
         0.5, -0.5,
         0.5,  0.5,
        
        -0.5, -0.5,
         0.5,  0.5,
        -0.5,  0.5
    };

    glVertexAttribPointer(m_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(m_program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(m_program.positionAttribute);
}

void Effects::start(EffectType effect_type, float effect_speed)
{
    m_current_effect = effect_type;
    m_effect_speed = effect_speed;

    switch (m_current_effect)
    {
        case NONE:                         break;
        case DIE:  m_alpha     = 1.0f;  break;
        case NEXT_LEVEL: m_alpha     = 1.0f;  break;
    }
}


void Effects::update(float delta_time)
{
   switch (m_current_effect)
   {
       case NONE: break;
           
       // Fades
       case DIE:
           m_alpha -= delta_time;
           if (m_alpha <= 0) m_current_effect = NONE;
           break;
       case NEXT_LEVEL:
           m_alpha -= delta_time;
           if (m_alpha <= 0) m_current_effect = NONE;
           break;
           
   }
}

void Effects::render()
{
    glm::mat4 model_matrix = glm::mat4(1.0f);

    switch (m_current_effect)
    {
        case NONE:
            break;
        case DIE:
            model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 0.0f));

            m_program.SetModelMatrix(model_matrix);
            


            m_program.SetColor(1.0f, 0.0f, 0.0f, m_alpha);

            draw_overlay();
            break;
        case NEXT_LEVEL:
            model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 0.0f));

            m_program.SetModelMatrix(model_matrix);

            m_program.SetColor(0.0f, 1.0f, 0.0f, m_alpha);
            draw_overlay();

            break;
    }
}
