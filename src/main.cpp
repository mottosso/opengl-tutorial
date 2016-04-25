#include <stdio.h>
#include <stdlib.h>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/ref/glfw.h"

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

    GLuint programId { LoadShaders("shader.vert", "shader.frag") };

    GLint matrixId { glGetUniformLocation(programId, "MVP") };

    static const GLfloat triangleBufferData[] {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f
    };

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

    GLfloat colorBufferData[12*3*3];
    for (int v = 0; v < 12*3; v++)
    {
        colorBufferData[3*v+0] = 1.0f / (rand() % 100);
        colorBufferData[3*v+1] = 1.0f / (rand() % 100);
        colorBufferData[3*v+2] = 1.0f / (rand() % 100);
    }

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertexBufferData),
                 vertexBufferData,
                 GL_STATIC_DRAW);

    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(colorBufferData),
                 colorBufferData,
                 GL_DYNAMIC_DRAW);

    GLuint triangleBuffer;
    glGenBuffers(1, &triangleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(triangleBufferData),
                 triangleBufferData,
                 GL_STATIC_DRAW);

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
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*) 0
        );

        for (int v = 0; v < 12*3; v++)
        {
            colorBufferData[3*v+0] = sin(glfwGetTime() + v * 0.1f);
            colorBufferData[3*v+1] = cos(glfwGetTime() + v * 0.1f);
            colorBufferData[3*v+2] = sin(glfwGetTime() + v * 0.1f + 0.5);
        }

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(colorBufferData),
                     colorBufferData,
                     GL_DYNAMIC_DRAW);

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)0
        );

        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        model = translate(mat4(1.0f), vec3(3.0f, 0.0f, 0.0f));
        model = rotate(model, radians(20.0f), vec3(0.0f, 1.0f, 0.0f));

        MVP = projection * view * model;
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)0
        );

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);

        ImGui::Render();

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteProgram(programId);
    glDeleteVertexArrays(1, &vertexArrayId);

    Shutdown();
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
