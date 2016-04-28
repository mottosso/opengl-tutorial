#include <stdio.h>
#include <stdlib.h>

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

// Temporary global variables
bool isMousePressed { false }; // Is the mouse currently pressed?
bool isMouseWithinWindow { false }; // Is the mouse currently within a window?
float mouseSensitivity = 0.5f;
double mouseLastX { 0 };
double mouseLastY { 0 };

float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

// Forward declarations
void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseMove(GLFWwindow* window, double xpos, double ypos);
void onMousePress(GLFWwindow* window, int button, int action, int mods);
inline vec3 positionFromMatrix(const mat4 & matrix);


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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0);  // Do not vsync

    gl3wInit();

    // ImGui initialisation
    Init(window, false);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    GLuint mVertexArrayId;
    glGenVertexArrays(1, &mVertexArrayId);
    glBindVertexArray(mVertexArrayId);

    GLuint mProgramId { Shader::Load("shader.vert", "shader.frag") };

    GLint mMatrixId { glGetUniformLocation(mProgramId, "MVP") };

    int w, h, comp;
    static const char* fname { "texture.jpg" };
    unsigned char* image = stbi_load(fname, &w, &h, &comp, STBI_rgb);

    if (image == nullptr)
    {
        fprintf(stderr, "Failed to load texture\n");
        exit(EXIT_FAILURE);
    }

    GLuint mTexture;
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);

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
    
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // Delete once uploaded
    stbi_image_free(image);

    GLuint mMySampler = glGetUniformLocation(mProgramId, "mySampler");

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

    GLuint mVertexBuffer;
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertexBufferData),
                 vertexBufferData,
                 GL_STATIC_DRAW);

    GLuint mUvBuffer;
    glGenBuffers(1, &mUvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mUvBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(uvBufferData),
                 uvBufferData,
                 GL_DYNAMIC_DRAW);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    bool mShowWindow { true };
    
    // Camera settings
    float mFov { 45.0f };
    float mNear { 0.1f };
    float mFar { 100.0f };

    // Object settings
    float mTranslateX { 0.0f };
    float mTranslateY { 0.0f };
    float mTranslateZ { 0.0f };
    float mRotateX { 0.0f };
    float mRotateY { 0.0f };
    float mRotateZ { 0.0f };
    float mScaleX { 1.0f };
    float mScaleY { 1.0f };
    float mScaleZ { 1.0f };

    // Close window on ESC
    glfwSetKeyCallback(window, onKeyPress);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetMouseButtonCallback(window, onMousePress);

    int displayWidth, displayHeight;
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(mProgramId);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(mMySampler, 0);

        NewFrame();

        mat4 projection { perspective(radians(mFov), 4.0f / 3.0f, mNear, mFar) };
        mat4 view { lookAt(
            vec3(4, 3, 3), // Camera position
            vec3(0, 0, 0), // Camera target
            vec3(0, 1, 0)  // Camera up-vector
        )};

        vec3 cameraPos { positionFromMatrix(view) };
        view = rotate(view, radians(verticalAngle), cross(cameraPos, vec3(0, -1, 0)));
        view = rotate(view, radians(horizontalAngle), vec3(0.0f, 1.0f, 0.0f));

        mat4 model { mat4(1.0f) };
        model = translate(model, vec3(mTranslateX, mTranslateY, mTranslateZ));
        model = rotate(model, radians(mRotateX), vec3(1.0f, 0.0f, 0.0f));
        model = rotate(model, radians(mRotateY), vec3(0.0f, 1.0f, 0.0f));
        model = rotate(model, radians(mRotateZ), vec3(0.0f, 0.0f, 1.0f));
        model = scale(model, vec3(mScaleX, mScaleY, mScaleZ));

        mat4 MVP { projection * view * model };

        ImGui::SetNextWindowSize({ 200, 130 }, ImGuiSetCond_FirstUseEver);

        ImGui::Begin("Camera", &mShowWindow);
        ImGui::SliderFloat("mFov", &mFov, 0.1f, 100.0f);
        ImGui::SliderFloat("mNear", &mNear, 0.1f, 20.0f);
        ImGui::SliderFloat("mFar", &mFar, 0.0f, 20.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::SetNextWindowSize({ 100, 300 }, ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Transform", &mShowWindow);
        ImGui::SliderFloat("tx", &mTranslateX, -10.0f, 10.0f);
        ImGui::SliderFloat("ty", &mTranslateY, -10.0f, 10.0f);
        ImGui::SliderFloat("tz", &mTranslateZ, -10.0f, 10.0f);
        ImGui::SliderFloat("rx", &mRotateX, -150.0f, 150.0f);
        ImGui::SliderFloat("ry", &mRotateY, -150.0f, 150.0f);
        ImGui::SliderFloat("rz", &mRotateZ, -150.0f, 150.0f);
        ImGui::SliderFloat("sx", &mScaleX, 0.0f, 3.0f);
        ImGui::SliderFloat("sy", &mScaleY, 0.0f, 3.0f);
        ImGui::SliderFloat("sz", &mScaleZ, 0.0f, 3.0f);
        ImGui::End();

        glUniformMatrix4fv(mMatrixId, 1, GL_FALSE, &MVP[0][0]);


        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glVertexAttribPointer(
            0,         // attribute
            3,         // dimensions
            GL_FLOAT,  // type
            GL_FALSE,  // normalised
            0,         // stride
            (void*) 0  // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, mUvBuffer);
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
        glfwPollEvents();
    }

    glDeleteBuffers(1, &mVertexBuffer);
    glDeleteBuffers(1, &mUvBuffer);
    glDeleteProgram(mProgramId);
    glDeleteVertexArrays(1, &mVertexArrayId);

    Shutdown();
    glfwTerminate();

    exit(EXIT_SUCCESS);
}


void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
}

void onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    if (isMousePressed)
    {
        double dx { xpos - mouseLastX };
        double dy { ypos - mouseLastY };

        horizontalAngle += mouseSensitivity * dx;
        verticalAngle += mouseSensitivity * dy;

        mouseLastX = xpos;
        mouseLastY = ypos;
    }
}

void onMousePress(GLFWwindow* window, int button, int action, int mods)
{
    glfwGetCursorPos(window, &mouseLastX, &mouseLastY);
    isMousePressed = !ImGui::IsMouseHoveringAnyWindow() && action == GLFW_PRESS;
}

/** Extract vec3 from mat4
 *  https://www.opengl.org/discussion_boards/showthread.php/178484-Extracting-camera-position-from-a-ModelView-Matrix
 */
inline vec3 positionFromMatrix(const mat4 & matrix)
{
  mat3 rotMat(matrix);
  vec3 d(matrix[3]);
 
  vec3 retVec = -d * rotMat;
  return retVec;
}
