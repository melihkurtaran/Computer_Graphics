#include <iostream>
#include <vector>

#define GLM_FORCE_LEFT_HANDED
#include "GLM/glm.hpp"
#include "GLM/common.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "GLM/gtx/rotate_vector.hpp"
#include "GLM/gtc/constants.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "opengl_utilities.h"
#include "mesh_generation.h"

using std::cout;
using std::endl;

/* Keep the global state inside this struct */
static struct {
    glm::dvec2 mouse_position;
    glm::ivec2 screen_dimensions = glm::ivec2(600, 600);
    int keyPressed;
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


    int vertical_segment = 160, rotation_segment = 160;
    
    std::vector<glm::vec3> positions;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    
    GenerateParametricShapeFrom2D(positions,normals,indices, uvs,ParametricHalfCircle,
                                  vertical_segment,rotation_segment);
    
    VAO MarsVAO(positions,normals,uvs,indices);
    
    positions.clear();
    indices.clear();
    normals.clear();
    GenerateParametricShapeFrom2D(positions,normals,indices,uvs,ParametricCircle,
                                  vertical_segment,rotation_segment);
    VAO torusVAO(positions,normals,uvs,indices);
    
    positions.clear();
    indices.clear();
    normals.clear();
    GenerateParametricShapeFrom2D(positions,normals,indices,uvs,ParametricHalfCircle,
                                  vertical_segment,rotation_segment);
    VAO sphereVAO(positions,normals,uvs,indices);
    
    stbi_set_flip_vertically_on_load(true);
    auto filename = "./mars_12k_color.jpg";
    int x,y,n;
    unsigned char *texture_mars = stbi_load(filename, &x, &y, &n, 0);
    
    if(texture_mars == NULL)
    {
        cout << "Texture " << filename << " failed to load." << endl;
        cout << "Error: " << stbi_failure_reason() << endl;
    }else{
        cout << "Texture " << filename << " is loaded." << endl;
        cout << "X: " << x << "Y: " << y << "N: " << n << endl;
    }
    
    GLuint texture;
    glGenTextures(1,&texture);
    
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            x,y,0,n==3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,texture_mars
            );
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);

    stbi_image_free(texture_mars);
    
    /* Creating Program */
    GLuint scene = CreateProgramFromSources(
        R"VERTEX(
#version 330 core
                                             
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;
                                             
uniform mat4 u_transform;
                                             
out vec3 vertex_position;
out vec3 vertex_normal;
out vec2 vertex_uv;
                                            
void main()
{
    gl_Position = u_transform * vec4(a_position, 1);
    vertex_normal = (u_transform * vec4(a_normal,0)).xyz;
    vertex_position = gl_Position.xyz;
    vertex_uv = a_uv;
}
        )VERTEX",

        R"FRAGMENT(
#version 330 core
  
uniform vec2 u_mouse_position;
uniform float u_material;
uniform sampler2D u_texture;
                                             
in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
                                             
out vec4 out_color;

void main()
{
  vec3 color = vec3(0);
  vec2 surface_uv = vertex_uv;
                                            
  vec3 surface_position = vertex_position;
  vec3 surface_normal = normalize(vertex_normal);
  vec3 surface_color;
  if (u_material == 1){
     surface_color = texture(u_texture,surface_uv).rgb;
  }else if(u_material == 2){
     surface_color = vec3(0.1);
  }else if(u_material == 3){
     surface_color = vec3(0,0.7,0);
  }else if(u_material == 4){
     surface_color = vec3(0.7,0,0);
  }
  float shininess = 128;

                                             
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

    if (scene == NULL)
    {
        glfwTerminate();
        return -1;
    }
    auto texture_location = glGetUniformLocation(scene, "u_texture");
    glUniform1i(texture_location,0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    
    auto u_mouse_position_location = glGetUniformLocation(scene, "u_mouse_position");
    auto u_transform_location = glGetUniformLocation(scene, "u_transform");
    auto material = glGetUniformLocation(scene, "u_material");
    
    /* Loop until the user closes the window */
    double X=0, Y=0, Z=0, Q=0, XC = 0, YC=0, ZC=0;
    bool cameraMode=false, collision = false;
    
    glm::vec3 chasing_pos = glm::vec3(0,0,0);
    glm::vec3 enemy1 = glm::vec3(5, 30.5, 346);
    glm::vec3 enemy2 = glm::vec3(-5, 30.5, 346);
    
    std::vector<glm::vec3> wheels_positions_enemy{
        glm::vec3(-6, 30, 345),
        glm::vec3(-4, 30, 345),
        glm::vec3(-6, 30, 347),
        glm::vec3(-4, 30, 347),
        
        glm::vec3(4, 30, 345),
        glm::vec3(6, 30, 345),
        glm::vec3(4, 30, 347),
        glm::vec3(6, 30, 347)
    };
    
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(scene);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        auto mouse_position = Globals.mouse_position / glm::dvec2(Globals.screen_dimensions);
        mouse_position.y = 1. - mouse_position.y;
        mouse_position = mouse_position * 2. - 1.;
        glUniform2fv(u_mouse_position_location, 1, glm::value_ptr(glm::vec2(mouse_position)));
       
        glm::mat4 camera_transform = glm::lookAt(
                           glm::vec3(X+XC,Y+YC,-2+Z+12+ZC), //eye location
                           glm::vec3(X+Q*Q*Q-mouse_position.x,Y+mouse_position.y,0), //center
                           glm::vec3(0,1,0)); //up
        
        if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) //camera mode turn ON
            cameraMode = true;
        if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) //camera mode turn OFF
        {
            cameraMode = false;
            XC=0;
            YC=0;
            ZC=0;
        }
        if(!collision)
        {
            if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !cameraMode) {X -= 0.001;}
            if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !cameraMode) { X += 0.001;}
            if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !cameraMode) {X -= 0.001;}
            if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !cameraMode) { X += 0.001;}
            if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !cameraMode) {Y += 0.001;}
            if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !cameraMode) {Y -= 0.001;}
            if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !cameraMode) {Z += 0.001;}
            if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !cameraMode) {Z -= 0.001;}
            
        }
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && cameraMode) {XC -= 0.01;}
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && cameraMode) { XC += 0.01;}
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && cameraMode) {XC -= 0.01;}
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && cameraMode) { XC += 0.01;}
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && cameraMode) {YC += 0.01;}
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && cameraMode) {YC -= 0.01;}
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && cameraMode) {ZC += 0.01;}
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && cameraMode) {ZC -= 0.01;}
        
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) //to look other ways
            Q += 0.02;
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            Q -= 0.02;
        
//        camera_transform = glm::inverse(camera_transform);

        auto projection = glm::perspective(glm::radians(45.f),1.f,0.1f,10.f);
        camera_transform = projection * camera_transform;
        camera_transform = glm::translate(camera_transform, glm::vec3(0,0,8));
        
        glm::mat4 transform = camera_transform;
//        transform = glm::translate(transform, glm::vec3(0,-1,0));
        transform = glm::translate(transform,glm::vec3(0+X,-0.79,1.6+Z));
        transform = glm::scale(transform, glm::vec3(0.8));
//        transform = glm::translate(transform,glm::vec3(-1,-1,-1));
        transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*0.01f)), glm::vec3(0,1,0));
        transform = glm::rotate(transform, glm::radians(float(-Z)*100), glm::vec3(1,0,0));
        transform = glm::rotate(transform, glm::radians(float(-X)*100), glm::vec3(0,1,0));
        auto t = u_transform_location;
        
        
        
        glBindVertexArray(MarsVAO.id);
        glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transform));
        glUniform1f(material,1);
        glDrawElements(GL_TRIANGLES, MarsVAO.element_array_count, GL_UNSIGNED_INT, NULL);
//        glDrawArrays(GL_LINE_STRIP,0,MarsVAO.element_array_count);
        

        glBindVertexArray(torusVAO.id);
        
        const auto draw_wheels = [&](glm::vec3 position)
        {
            glm::mat4 transformW = camera_transform;
            transformW = glm::translate(transformW, chasing_pos);
            transformW = glm::scale(transformW, glm::vec3(0.005));
            auto translate = glm::translate(position);
            transformW *= translate;
            transformW = glm::rotate(transformW, glm::radians(90.f),glm::vec3(1,0,0));
            transformW = glm::rotate(transformW, glm::radians(90.f),glm::vec3(0,0,1));
            
            transformW = glm::rotate(transformW, glm::radians(float(Z)*1000),glm::vec3(0,1,0));
            transformW = glm::rotate(transformW, glm::radians(float(sin(X/10))*1000),glm::vec3(1,1,0));
            glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transformW));
            glUniform1f(material,2);
            glDrawElements(GL_TRIANGLES, torusVAO.element_array_count, GL_UNSIGNED_INT, NULL);
        };
        
        const std::vector<glm::vec3> wheels_positions{
            glm::vec3(-1, -3, 370),
            glm::vec3(1, -3, 370),
            glm::vec3(-1, -3, 372),
            glm::vec3(1, -3, 372)
        };
        for (const auto& p : wheels_positions)
            draw_wheels(p);
        
       
        glBindVertexArray(sphereVAO.id);
        glm::mat4 transformW = camera_transform;
        transformW = glm::translate(transformW, glm::vec3(X,Y,Z));
        transformW = glm::scale(transformW, glm::vec3(0.005));
        auto translate = glm::translate(glm::vec3(0, -2.5, 371));
        transformW *= translate;
        transformW = glm::rotate(transformW, glm::radians(90.f),glm::vec3(1,0,0));
        transformW = glm::rotate(transformW, glm::radians(90.f),glm::vec3(0,0,1));
        
        glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transformW));
        glUniform1f(material,3);
        glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
        
        /*Enemy Rovers**/
        glBindVertexArray(torusVAO.id);
        auto rover_pos = glm::vec3(X*200,Y*200-2.5,200*Z+371);
        
        const auto draw_wheels_enemy = [&](glm::vec3 position)
        {
            glm::mat4 transformW = camera_transform;
            transformW = glm::scale(transformW, glm::vec3(0.005));
//            transformW = glm::translate(transformW, enemy1);
            auto translate = glm::translate(position);
            transformW *= translate;
            transformW = glm::rotate(transformW, glm::radians(90.f),glm::vec3(1,0,0));
            transformW = glm::rotate(transformW, glm::radians(90.f),glm::vec3(0,0,1));
            
            transformW = glm::rotate(transformW, glm::radians(float(Z)*1000),glm::vec3(0,1,0));
            transformW = glm::rotate(transformW, glm::radians(float(sin(X))*100),glm::vec3(0,0,1));

            glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transformW));
            glUniform1f(material,2);
            glDrawElements(GL_TRIANGLES, torusVAO.element_array_count, GL_UNSIGNED_INT, NULL);
        };
        
        int counter = 0;
        for (auto& p : wheels_positions_enemy)
        {
            if(counter<4)
                p = glm::mix(p, 200.f*glm::vec3(X,Y,Z) + wheels_positions[counter], 0.004);
            else
                p = glm::mix(p, 200.f*glm::vec3(X,Y,Z) + wheels_positions[counter-4], 0.006);
            draw_wheels_enemy(p);
            counter++;
        }
        
       
        glBindVertexArray(sphereVAO.id);
        auto transformW2 = camera_transform;
        transformW2 = glm::translate(transformW2, glm::vec3(0));
        chasing_pos = glm::vec3(X, Y, Z);
        
        transformW2 = glm::scale(transformW2, glm::vec3(0.005));
        enemy1 = glm::mix(enemy1, rover_pos, 0.006);
        enemy2 = glm::mix(enemy2, rover_pos, 0.004);
        auto translate3 = glm::translate(enemy1);
        auto translate2 = glm::translate(enemy2);
        if(pow(enemy1.x-rover_pos.x,2) + pow(enemy1.y-rover_pos.y,2) + pow(enemy1.z-rover_pos.z,2)<= 4)
            collision = true;
        else if(pow(enemy2.x-rover_pos.x,2) + pow(enemy2.y-rover_pos.y,2) + pow(enemy2.z-rover_pos.z,2)<= 4)
            collision = true;
        
        auto transformW3 = transformW2 * translate3;
        transformW2 *= translate2;
        
        
        glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transformW2));
        glUniform1f(material,4);
        glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
        glUniformMatrix4fv(t,1,GL_FALSE,glm::value_ptr(transformW3));
        glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
