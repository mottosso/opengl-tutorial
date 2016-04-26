#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/ref/glfw.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"

using namespace glm;


int main(void)
{
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    static GLFWwindow* window {
        glfwCreateWindow(800, 600, "Simple example", NULL, NULL) 
    };

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE , GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);  // Do not vsync

    gl3wInit();

    // ImGui initialisation
    Init(window, false);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    GLuint programId { Shader::Load("shader.vert", "shader.frag") };

    GLint matrixId { glGetUniformLocation(programId, "MVP") };

    int w, h, comp;
    static const char* fname { "texture.jpg" };
    unsigned char* image = stbi_load(fname, &w, &h, &comp, STBI_rgb);

    if (image == nullptr)
    {
        fprintf(stderr, "Failed to load texture\n");
        exit(EXIT_FAILURE);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    if      (comp == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,  w, h, 0, GL_RGB,  GL_UNSIGNED_BYTE, image);
    else if (comp == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    else
    {
        fprintf(stderr, "Unsupported number of channels in %s: %i\n", fname, comp);
        exit(EXIT_FAILURE);;
    }
    
    glBindTexture(GL_TEXTURE_2D, texture);

    GLuint mySampler = glGetUniformLocation(programId, "mySampler");

    // A cube
    static const GLfloat vertexBufferData[] {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f
    };

    static const GLfloat uvBufferData[] = { 
        0.000059f, 1.0f-0.000004f, 
        0.000103f, 1.0f-0.336048f, 
        0.335973f, 1.0f-0.335903f, 
        1.000023f, 1.0f-0.000013f, 
        0.667979f, 1.0f-0.335851f, 
        0.999958f, 1.0f-0.336064f, 
        0.667979f, 1.0f-0.335851f, 
        0.336024f, 1.0f-0.671877f, 
        0.667969f, 1.0f-0.671889f, 
        1.000023f, 1.0f-0.000013f, 
        0.668104f, 1.0f-0.000013f, 
        0.667979f, 1.0f-0.335851f, 
        0.000059f, 1.0f-0.000004f, 
        0.335973f, 1.0f-0.335903f, 
        0.336098f, 1.0f-0.000071f, 
        0.667979f, 1.0f-0.335851f, 
        0.335973f, 1.0f-0.335903f, 
        0.336024f, 1.0f-0.671877f, 
        1.000004f, 1.0f-0.671847f, 
        0.999958f, 1.0f-0.336064f, 
        0.667979f, 1.0f-0.335851f, 
        0.668104f, 1.0f-0.000013f, 
        0.335973f, 1.0f-0.335903f, 
        0.667979f, 1.0f-0.335851f, 
        0.335973f, 1.0f-0.335903f, 
        0.668104f, 1.0f-0.000013f, 
        0.336098f, 1.0f-0.000071f, 
        0.000103f, 1.0f-0.336048f, 
        0.000004f, 1.0f-0.671870f, 
        0.336024f, 1.0f-0.671877f, 
        0.000103f, 1.0f-0.336048f, 
        0.336024f, 1.0f-0.671877f, 
        0.335973f, 1.0f-0.335903f, 
        0.667969f, 1.0f-0.671889f, 
        1.000004f, 1.0f-0.671847f, 
        0.667979f, 1.0f-0.335851f
    };

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertexBufferData),
                 vertexBufferData,
                 GL_STATIC_DRAW);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(uvBufferData),
                 uvBufferData,
                 GL_DYNAMIC_DRAW);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    bool showWindow { true };
    
    // Camera settings
    float fov { 45.0f };
    float near { 0.1f };
    float far { 100.0f };
    float dolly { 0.0f };
    float translateX { 0.0f };
    float translateY { 0.0f };
    float translateZ { 0.0f };
    float rotateX { 0.0f };
    float rotateY { 0.0f };
    float rotateZ { 0.0f };
    float scaleX { 1.0f };
    float scaleY { 1.0f };
    float scaleZ { 1.0f };

    // Close window on ESC
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    });

    int displayWidth, displayHeight;
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programId);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(mySampler, 0);

        NewFrame();

        mat4 projection { perspective(radians(fov), 4.0f / 3.0f, near, far) };
        mat4 view { lookAt(
            vec3(4, 3, 3),
            vec3(0, 0, 0),
            vec3(0, 1, 0)
        )};

        view = rotate(view, radians(dolly), vec3(0.0f, 1.0f, 0.0f));

        mat4 model { mat4(1.0f) };
        model = translate(model, vec3(translateX, translateY, translateZ));
        model = rotate(model, radians(rotateX), vec3(1.0f, 0.0f, 0.0f));
        model = rotate(model, radians(rotateY), vec3(0.0f, 1.0f, 0.0f));
        model = rotate(model, radians(rotateZ), vec3(0.0f, 0.0f, 1.0f));
        model = scale(model, vec3(scaleX, scaleY, scaleZ));

        mat4 MVP { projection * view * model };

        ImGui::SetNextWindowSize({ 200, 130 }, ImGuiSetCond_FirstUseEver);

        ImGui::Begin("Camera", &showWindow);
        ImGui::SliderFloat("fov", &fov, 0.1f, 100.0f);
        ImGui::SliderFloat("near", &near, 0.1f, 20.0f);
        ImGui::SliderFloat("far", &far, 0.0f, 20.0f);
        ImGui::SliderFloat("dolly", &dolly, -50.0f, 50.0f);
        ImGui::End();

        ImGui::SetNextWindowSize({ 100, 300 }, ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Transform", &showWindow);
        ImGui::SliderFloat("tx", &translateX, -10.0f, 10.0f);
        ImGui::SliderFloat("ty", &translateY, -10.0f, 10.0f);
        ImGui::SliderFloat("tz", &translateZ, -10.0f, 10.0f);
        ImGui::SliderFloat("rx", &rotateX, -150.0f, 150.0f);
        ImGui::SliderFloat("ry", &rotateY, -150.0f, 150.0f);
        ImGui::SliderFloat("rz", &rotateZ, -150.0f, 150.0f);
        ImGui::SliderFloat("sx", &scaleX, 0.0f, 3.0f);
        ImGui::SliderFloat("sy", &scaleY, 0.0f, 3.0f);
        ImGui::SliderFloat("sz", &scaleZ, 0.0f, 3.0f);
        ImGui::End();

        glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);

        glfwPollEvents();

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(
            0,         // attribute
            3,         // dimensions
            GL_FLOAT,  // type
            GL_FALSE,  // normalised
            0,         // stride
            (void*) 0  // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glVertexAttribPointer(
            1,
            2,          // size : U+V == 2
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *) 0
        );

        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        ImGui::Render();

        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteProgram(programId);
    glDeleteVertexArrays(1, &vertexArrayId);

    Shutdown();
    glfwTerminate();
    stbi_image_free(image);

    exit(EXIT_SUCCESS);
}
