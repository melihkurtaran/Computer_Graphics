#include <iostream>
#include <vector>

#include "GLM/glm.hpp"
#include "GLM/common.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "GLM/gtx/rotate_vector.hpp"
#include "GLM/gtc/constants.hpp"

#include "opengl_utilities.h"
#include "mesh_generation.h"

using std::cout;
using std::endl;

/* Keep the global state inside this struct */
static struct {
    glm::dvec2 mouse_position;
    glm::ivec2 screen_dimensions = glm::ivec2(600, 600);
    int keyPressed = GLFW_KEY_Q;
} Globals;

/* GLFW Callback functions */
static void ErrorCallback(int error, const char* description)
{
    std::cerr << "Error: " << description << endl;
}

static void CursorPositionCallback(GLFWwindow* window, double x, double y)
{
    Globals.mouse_position.x = x;
    Globals.mouse_position.y = y;
}

static void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    Globals.screen_dimensions.x = width;
    Globals.screen_dimensions.y = height;

    glViewport(0, 0, width, height);
}

static void Keycallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Globals.keyPressed = key;
}

int main(int argc, char* argv[])
{
    /* Set GLFW error callback */
    glfwSetErrorCallback(ErrorCallback);

    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(
        Globals.screen_dimensions.x, Globals.screen_dimensions.y,
        "Melih Kurtaran", NULL, NULL
    );
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    /* Move window to a certain position [do not change] */
    glfwSetWindowPos(window, 10, 50);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /* Enable VSync */
    glfwSwapInterval(1);

    /* Load OpenGL extensions with GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Set GLFW Callbacks */
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    glfwSetKeyCallback(window, Keycallback);
    
    /* Configure OpenGL */
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);


    int vertical_segment = 16, rotation_segment = 16;
    
    std::vector<glm::vec3> positions;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> normals;
    
    
    GenerateParametricShapeFrom2D(positions,normals,indices,ParametricHalfCircle,
                                  vertical_segment,rotation_segment);
    VAO sphereVAO(positions,normals,indices);
    
    positions.clear();
    indices.clear();
    normals.clear();
    GenerateParametricShapeFrom2D(positions,normals,indices,ParametricCircle,
                                  vertical_segment,rotation_segment);
    VAO torusVAO(positions,normals,indices);
    
    positions.clear();
    indices.clear();
    normals.clear();
    
    GenerateParametricShapeFrom2D(positions,normals,indices,ParametricShape3,
                                  vertical_segment*8,rotation_segment*8);
    VAO shape3VAO(positions,normals,indices);
    
    positions.clear();
    indices.clear();
    normals.clear();
    
    GenerateParametricShapeFrom2D(positions,normals,indices,ParametricShape4,
                                  vertical_segment*4,rotation_segment*4);
    VAO shape4VAO(positions,normals,indices);
    
    /* Creating Programs */
    GLuint scene1 = CreateProgramFromSources(
        R"VERTEX(
#version 330 core

layout(location = 0) in vec3 a_position;
                                             
uniform mat4 u_transform;

void main()
{
    gl_Position = u_transform * vec4(a_position, 1);
}
        )VERTEX",

        R"FRAGMENT(
#version 330 core

out vec4 out_color;

void main()
{
    out_color = vec4(1, 1, 1, 1);
}
        )FRAGMENT");
    
    GLuint scene2 = CreateProgramFromSources(
        R"VERTEX(
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
                                             
uniform mat4 u_transform;
                                             
out vec3 vertex_position;
out vec3 vertex_normal;
void main()
{
    gl_Position = u_transform * vec4(a_position, 1);
    vertex_normal = (u_transform * vec4(a_normal,0)).xyz;
    vertex_position = gl_Position.xyz;
}
        )VERTEX",

        R"FRAGMENT(
#version 330 core
                                                      
in vec3 vertex_position;
in vec3 vertex_normal;

out vec4 out_color;

void main()
{

  float ambient = 2;
  vec3 ambient_color = vec3(1,1,1);
                                             
  vec3 result = (ambient_color*ambient)* vertex_normal;
  out_color = vec4(result,1);
}
        )FRAGMENT");
    GLuint scene3 = CreateProgramFromSources(
        R"VERTEX(
#version 330 core
                                             
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
                                             
uniform mat4 u_transform;
                                             
out vec3 vertex_position;
out vec3 vertex_normal;
void main()
{
    gl_Position = u_transform * vec4(a_position, 1);
    vertex_normal = (u_transform * vec4(a_normal,0)).xyz;
    vertex_position = gl_Position.xyz;
}
        )VERTEX",

        R"FRAGMENT(
#version 330 core
                                             
in vec3 vertex_position;
in vec3 vertex_normal;

out vec4 out_color;

void main()
{
 vec3 color = vec3(0);
 vec3 surface_color = vec3(0.5,0.5,0.5);
 vec3 surface_position = vertex_position;
 vec3 surface_normal = normalize(vertex_normal);
                                            
 vec3 ambient_color = vec3(0.5,0.5,0.5);
 color += ambient_color * surface_color;
                                            
 vec3 light_direction = vec3(-1,-1,1);
 vec3 light_color = vec3(0.4,0.4,0.4);
                                            
 float diffuse_intensity = max(0,dot(-1*light_direction,surface_normal));
 color += diffuse_intensity * light_color * surface_color;
                                            
 vec3 view_dir = normalize(vec3(vertex_position.xy,-1)-surface_position);
 vec3 halfway_dir = normalize(view_dir + -1*light_direction);
 float shininess = 64;
 float specular_intensity = max(0, dot(halfway_dir, surface_normal));
 color += pow(specular_intensity, shininess) * light_color;
                                            
 out_color = vec4(color,1);
}
        )FRAGMENT");
    GLuint scene4 = CreateProgramFromSources(
        R"VERTEX(
#version 330 core
                                             
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
                                             
uniform mat4 u_transform;
                                             
out vec3 vertex_position;
out vec3 vertex_normal;
void main()
{
    gl_Position = u_transform * vec4(a_position, 1);
    vertex_normal = (u_transform * vec4(a_normal,0)).xyz;
    vertex_position = gl_Position.xyz;
}
        )VERTEX",

        R"FRAGMENT(
#version 330 core
  
uniform vec2 u_mouse_position;
uniform vec3 u_color;
uniform float u_shininess;
                                             
in vec3 vertex_position;
in vec3 vertex_normal;
                                             
out vec4 out_color;

void main()
{
  vec3 color = vec3(0);
  vec3 surface_color = u_color;
  float shininess = u_shininess;
                                             
//  if(vertex_position.x <= 0 && vertex_position.y >= 0) {  shininess = 128; }
//  else if(vertex_position.x >= 0 && vertex_position.y >= 0) {  shininess = 32; }
//  else if(vertex_position.x <= 0 && vertex_position.y <= 0) {  shininess = 256; }
//  else if(vertex_position.x >= 0 && vertex_position.y >= 0) {  shininess = 8; }

  vec3 surface_position = vertex_position;
  vec3 surface_normal = normalize(vertex_normal);
                                             
  vec3 ambient_color = vec3(0.5,0.5,0.5);
  color += ambient_color * surface_color;
                                             
  vec3 light_position = vec3(u_mouse_position,-1);
  vec3 to_light = normalize(light_position - surface_position);
  vec3 light_direction2 = vec3(1,1,-1);
  vec3 light_color = vec3(0.4,0.4,0.4);
                                             
  float diffuse_intensity = max(0,dot(to_light,surface_normal));
  float diffuse_intensity2 = max(0,dot(light_direction2,surface_normal));
  color += diffuse_intensity * light_color * surface_color;
  color += diffuse_intensity2 * light_color * surface_color;
                                             
  vec3 view_dir = normalize(vec3(vertex_position.xy,-1)-surface_position);
  vec3 halfway_dir = normalize(view_dir + to_light);
  vec3 halfway_dir2 = normalize(view_dir + light_direction2);
  float specular_intensity = max(0, dot(halfway_dir, surface_normal));
  float specular_intensity2 = max(0, dot(halfway_dir2, surface_normal));
  color += pow(specular_intensity, shininess) * light_color;
  color += pow(specular_intensity2, shininess) * light_color;
                                             
  out_color = vec4(color,1);
}
        )FRAGMENT");
    GLuint scene5 = CreateProgramFromSources(
        R"VERTEX(
#version 330 core

 layout(location = 0) in vec3 a_position;
 layout(location = 1) in vec3 a_normal;
                                              
 uniform mat4 u_transform;
                                              
 out vec3 vertex_position;
 out vec3 vertex_normal;
 void main()
 {
     gl_Position = u_transform * vec4(a_position, 1);
     vertex_normal = (u_transform * vec4(a_normal,0)).xyz;
     vertex_position = gl_Position.xyz;
 }
         )VERTEX",

         R"FRAGMENT(
 #version 330 core
   
 uniform vec2 u_mouse_position;
 uniform vec3 u_color;
                                             
 in vec3 vertex_position;
 in vec3 vertex_normal;
 out vec4 out_color;

 void main()
 {
 vec3 surface_color = u_color;
                                              
 vec3 color = vec3(0);
 vec3 surface_position = vertex_position;
 vec3 surface_normal = normalize(vertex_normal);
                                            
 vec3 ambient_color = vec3(0.5,0.5,0.5);
 color += ambient_color * surface_color;
                                            
 vec3 light_direction = vec3(u_mouse_position,-1);
 vec3 light_direction2 = vec3(1,1,-1);
 vec3 light_color = vec3(0.4,0.4,0.4);
                                            
 float diffuse_intensity = max(0,dot(light_direction,surface_normal));
 float diffuse_intensity2 = max(0,dot(light_direction2,surface_normal));
 color += diffuse_intensity * light_color * surface_color;
 color += diffuse_intensity2 * light_color * surface_color;
                                            
 vec3 view_dir = normalize(vec3(vertex_position.xy,-1)-surface_position);
 vec3 halfway_dir = normalize(view_dir + light_direction);
 vec3 halfway_dir2 = normalize(view_dir + light_direction2);
 float shininess = 64;
 float specular_intensity = max(0, dot(halfway_dir, surface_normal));
 float specular_intensity2 = max(0, dot(halfway_dir2, surface_normal));
 color += pow(specular_intensity, shininess) * light_color;
 color += pow(specular_intensity2, shininess) * light_color;
                                              
   out_color = vec4(color,1);
 }
         )FRAGMENT");
    GLuint scene6 = CreateProgramFromSources(
         R"VERTEX(
 #version 330 core

 layout(location = 0) in vec3 a_position;
 layout(location = 1) in vec3 a_normal;
                                              
 uniform mat4 u_transform;
                                              
 out vec3 vertex_position;
 out vec3 vertex_normal;
 void main()
 {
     gl_Position = u_transform * vec4(a_position, 1);
     vertex_normal = (u_transform * vec4(a_normal,0)).xyz;
     vertex_position = gl_Position.xyz;
 }
         )VERTEX",

         R"FRAGMENT(
 #version 330 core
                                                       
 in vec3 vertex_position;
 in vec3 vertex_normal;

 out vec4 out_color;

 void main()
 {

   float ambient = 2;
   vec3 ambient_color = vec3(1,1,1);
                                              
   vec3 result = (ambient_color*ambient)* vertex_normal;
   out_color = vec4(result,1);
 }
         )FRAGMENT");
    if (scene1 == NULL || scene2 == NULL || scene3 == NULL
        || scene4 == NULL || scene5 == NULL || scene6 == NULL)
    {
        glfwTerminate();
        return -1;
    }
    auto u_mouse_position_location = glGetUniformLocation(scene4, "u_mouse_position");
    auto u_transform_location = glGetUniformLocation(scene1, "u_transform");
    auto u_transform_location_s4 = glGetUniformLocation(scene4, "u_transform");
    auto u_transform_location_s5 = glGetUniformLocation(scene5, "u_transform");
    auto color_location = glGetUniformLocation(scene4, "u_color");
    auto shininess = glGetUniformLocation(scene4, "u_shininess");
    
    /* Loop until the user closes the window */
    
    glm::vec3 chasing_pos = glm::vec3(0,0,0);
    
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(Globals.keyPressed == GLFW_KEY_Q)
        {
            glUseProgram(scene1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if(Globals.keyPressed == GLFW_KEY_W)
        {
            glUseProgram(scene2);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else if(Globals.keyPressed == GLFW_KEY_E)
        {
            glUseProgram(scene3);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else if(Globals.keyPressed == GLFW_KEY_R)
        {
            glUseProgram(scene4);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            auto mouse_position = Globals.mouse_position / glm::dvec2(Globals.screen_dimensions);
            mouse_position.y = 1. - mouse_position.y;
            mouse_position = mouse_position * 2. - 1.;
            glUniform2fv(u_mouse_position_location, 1, glm::value_ptr(glm::vec2(mouse_position)));
        }
        
        if(Globals.keyPressed == GLFW_KEY_Q || Globals.keyPressed == GLFW_KEY_W
           || Globals.keyPressed == GLFW_KEY_E || Globals.keyPressed == GLFW_KEY_R)
        {
            glm::mat4 transform(1.0);
            transform = glm::scale(transform, glm::vec3(0.4));
            transform = glm::translate(transform, glm::vec3(-1.2,1.2,0));
            auto transform3 = glm::translate(transform, glm::vec3(0,-2.4,0));
            auto transform2 = glm::translate(transform, glm::vec3(2.4,0,0));
            auto transform4 = glm::translate(transform2, glm::vec3(0,-2.4,0));
            transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*10)), glm::vec3(1,1,0));
            transform2 = glm::rotate(transform2, glm::radians(float(glfwGetTime()*10)), glm::vec3(1,1,0));
            transform3 = glm::rotate(transform3, glm::radians(float(glfwGetTime()*10)), glm::vec3(1,1,0));
            transform4 = glm::rotate(transform4, glm::radians(float(glfwGetTime()*10)), glm::vec3(1,1,0));
            
            auto t = u_transform_location;
            if(Globals.keyPressed == GLFW_KEY_R)
                t = u_transform_location_s4;
            
            glBindVertexArray(sphereVAO.id);
            
            glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transform));
            glUniform3fv(color_location,1,glm::value_ptr(glm::vec3(0.5,0.5,0.5)));
            glUniform1f(shininess,128);
            glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    //        glDrawArrays(GL_LINE_STRIP,0,sphereVAO.element_array_count);
            
            glBindVertexArray(torusVAO.id);
            glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transform2));
            glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(1,0,0)));
            glUniform1f(shininess,32);
            glDrawElements(GL_TRIANGLES, torusVAO.element_array_count, GL_UNSIGNED_INT, NULL);
            
            glBindVertexArray(shape3VAO.id);
            glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transform3));
            glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,0)));
            glUniform1f(shininess,256);
            glDrawElements(GL_TRIANGLES, shape3VAO.element_array_count, GL_UNSIGNED_INT, NULL);
            
            glBindVertexArray(shape4VAO.id);
            glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transform4));
            glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,0,1)));
            glUniform1f(shininess,8);
            glDrawElements(GL_TRIANGLES, shape4VAO.element_array_count, GL_UNSIGNED_INT, NULL);
        }else if(Globals.keyPressed == GLFW_KEY_T)
        {
            glUseProgram(scene5);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glm::mat4 transform(1.0);
            glm::mat4 transformChasing(1.0);
            
            auto mouse_position = Globals.mouse_position / glm::dvec2(Globals.screen_dimensions);
            mouse_position.y = 1. - mouse_position.y;
            mouse_position = mouse_position * 2. - 1.;
            
            chasing_pos = glm::mix(glm::vec3(mouse_position,0), chasing_pos, 0.99);
            
            transformChasing = glm::translate(transformChasing, chasing_pos);
            transformChasing = glm::scale(transformChasing, glm::vec3(0.3));
            
            transform = glm::translate(transform, glm::vec3(mouse_position,0));
            transform = glm::scale(transform, glm::vec3(0.3));
            
            if((pow(chasing_pos.x-mouse_position.x,2) + pow(chasing_pos.y-mouse_position.y,2)) <= 0.36)
                glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(1,0,0)));
            else
                glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,0)));
            
            glUniformMatrix4fv(u_transform_location_s5,1,GL_FALSE,glm::value_ptr(transform));
            glBindVertexArray(sphereVAO.id);
            glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, 0);
            
            glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0.5,0.5,0.5)));
            glUniformMatrix4fv(u_transform_location_s5,1,GL_FALSE,glm::value_ptr(transformChasing));
            glBindVertexArray(sphereVAO.id);
            glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, 0);
            
        }else 
        {
            glUseProgram(scene6);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glm::mat4 transform(1.0);
            transform *= glm::scale(glm::vec3(0.3));
            transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*30)), glm::vec3(0.7,0.2,0));
            glBindVertexArray(shape4VAO.id);
            glUniformMatrix4fv(u_transform_location,1,GL_FALSE,glm::value_ptr(transform));
            glDrawElements(GL_TRIANGLES, shape4VAO.element_array_count, GL_UNSIGNED_INT, NULL);
            
            for(int i=0; i< 4 ; i++)
            {
                glm::mat4 transform2(1.0), transformTorus(1.0);
                transform2 = glm::rotate(transform2, glm::radians(float(glfwGetTime()*40)), glm::vec3(0,0,1));
                switch (i) {
                    case 0:
                        transform2 *= glm::translate(glm::vec3(-0.7, 0, 0));
                        break;
                    case 1:
                        transform2 *= glm::translate(glm::vec3(0, 0.7, 0));
                        break;
                    case 2:
                        transform2 *= glm::translate(glm::vec3(0, -0.7, 0));
                        break;
                    case 3:
                        transform2 *= glm::translate(glm::vec3(0.7, 0, 0));
                        break;
                    default:
                        break;
                }
                transform2 *= glm::scale(glm::vec3(0.2));
                transform2 = glm::rotate(transform2, glm::radians(float(glfwGetTime()*100)), glm::vec3(0.4,1,0));
                glBindVertexArray(shape3VAO.id);

                glUniformMatrix4fv(u_transform_location,1,GL_FALSE,glm::value_ptr(transform2));
                glDrawElements(GL_TRIANGLES, shape3VAO.element_array_count, GL_UNSIGNED_INT, NULL);
                
                transformTorus = glm::rotate(transformTorus, glm::radians(float(glfwGetTime()) * 100), glm::vec3(1, 0, 1));
                transformTorus = glm::translate(transformTorus, glm::vec3(0, 1, 0));
                transformTorus *= glm::scale(glm::vec3(0.8));
                transformTorus = transform2 * transformTorus;
                
                glBindVertexArray(torusVAO.id);
                glUniformMatrix4fv(u_transform_location,1,GL_FALSE,glm::value_ptr(transformTorus));
                glDrawElements(GL_TRIANGLES, torusVAO.element_array_count, GL_UNSIGNED_INT, NULL);
            }
        }
            
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
