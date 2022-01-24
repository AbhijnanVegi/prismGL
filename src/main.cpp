#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "shader.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void moveModel(int dir, float deltaTime);
void resetState();

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// Half the len of prism
const float prismLen = 0.5f;

// Init Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// Globals
glm::mat4 model, view, projection;
glm::vec3 pos;
float angle;
bool outOfPlace = false;
bool modelSpin = false;
bool camSpin = false;

int main(int argc, char **argv)
{
    // Validate args
    if (argc != 2)
    {
        std::cout << "Usage : ./app <n>" << std::endl;
        exit(0);
    }
    // Prism n sides
    int pn = atoi(argv[1]);
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MyGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Register function to handle viewport with change in dimensions

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // OpenGL Config
    // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_BLEND);
    // Enable depth
    glEnable(GL_DEPTH_TEST);

    // Compiler shaders
    Shader ourShader("../src/vertex.shader", "../src/fragment.shader");

    // Vertices
    float vertices[4 * pn][18];
    for (int i = 0; i < pn; i++)
    {
        vertices[i][0] = 0;
        vertices[i][1] = 0;
        vertices[i][2] = prismLen;
        vertices[i][3] = 1;
        vertices[i][4] = 1;
        vertices[i][5] = 0;
        vertices[i][6] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][7] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][8] = prismLen;
        vertices[i][9] = 1;
        vertices[i][10] = 1;
        vertices[i][11] = 0;
        vertices[i][12] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][13] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][14] = prismLen;
        vertices[i][15] = 1;
        vertices[i][16] = 1;
        vertices[i][17] = 0;
    }
    for (int i = pn; i < 2 * pn; i++)
    {
        vertices[i][0] = 0;
        vertices[i][1] = 0;
        vertices[i][2] = -prismLen;
        vertices[i][3] = 0;
        vertices[i][4] = 1;
        vertices[i][5] = 1;
        vertices[i][6] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][7] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][8] = -prismLen;
        vertices[i][9] = 0;
        vertices[i][10] = 1;
        vertices[i][11] = 1;
        vertices[i][12] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][13] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][14] = -prismLen;
        vertices[i][15] = 0;
        vertices[i][16] = 1;
        vertices[i][17] = 1;
    }
    for (int i = 2 * pn; i < 3 * pn; i++)
    {
        vertices[i][0] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][1] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][2] = prismLen;
        vertices[i][3] = (1.0f / pn) * (i % pn);
        vertices[i][4] = 0;
        vertices[i][5] = (1.0f / pn) * (i % pn);
        vertices[i][6] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][7] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][8] = -prismLen;
        vertices[i][9] = (1.0f / pn) * (i % pn);
        vertices[i][10] = 0;
        vertices[i][11] = (1.0f / pn) * (i % pn);
        vertices[i][12] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][13] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][14] = -prismLen;
        vertices[i][15] = (1.0f / pn) * (i % pn);
        vertices[i][16] = 0;
        vertices[i][17] = (1.0f / pn) * (i % pn);
    }
    for (int i = 3 * pn; i < 4 * pn; i++)
    {
        vertices[i][0] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][1] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][2] = -prismLen;
        vertices[i][3] = (1.0f / pn) * (i % pn);
        vertices[i][4] = 0;
        vertices[i][5] = (1.0f / pn) * (i % pn);
        vertices[i][6] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][7] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][8] = prismLen;
        vertices[i][9] = (1.0f / pn) * (i % pn);
        vertices[i][10] = 0;
        vertices[i][11] = (1.0f / pn) * (i % pn);
        vertices[i][12] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][13] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][14] = prismLen;
        vertices[i][15] = (1.0f / pn) * (i % pn);
        vertices[i][16] = 0;
        vertices[i][17] = (1.0f / pn) * (i % pn);
    }
    // Init object specifics
    pos = glm::vec3(0, 0, 0);
    angle = 0;

    // Gpu buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);                                                         // Init VAO
    glGenBuffers(1, &VBO);                                                              // Generates vertex buffers
    glBindVertexArray(VAO);                                                             // Bind VBO, VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                                 // Binds buffers to VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (float *)vertices, GL_STATIC_DRAW); // Uploads data to GPU

    // Link vertex array
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // Tells openGL how to interpret the data
    glEnableVertexAttribArray(0);                                                  // Enables vertex attribute array

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unneccecary
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindVertexArray(0);             // Unbind VAO

    // Position of prism top faces
    glm::vec3 topPos[] =
        {
            glm::vec3(0, 0, prismLen),
            glm::vec3(0, 0, -prismLen)};

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Input handling
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Bg color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();        // Use shaders
        glBindVertexArray(VAO); // Bind VAO

        model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0, 0));
        // Perspective and view
        view = camera.GetViewMatrix();

        projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        // projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        for (int i = 0; i < 2; i++)
        {
            glDrawArrays(GL_TRIANGLES, 0, 12 * pn); // Draw Triangle
        }

        // Neccessary stuff
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (outOfPlace)
            resetState();
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (outOfPlace)
            resetState();
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if (outOfPlace)
            resetState();
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if (outOfPlace)
            resetState();
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (outOfPlace)
            resetState();
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (outOfPlace)
            resetState();
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        outOfPlace = true;
        moveModel(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        outOfPlace = true;
        moveModel(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        outOfPlace = true;
        moveModel(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        outOfPlace = true;
        moveModel(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        outOfPlace = true;
        moveModel(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        outOfPlace = true;
        moveModel(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        modelSpin = !modelSpin;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (outOfPlace)
            resetState();
        camera.Position = glm::vec3(0, 0, 2);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (outOfPlace)
            resetState();
        camera.Position = glm::vec3(0, 0, -2);
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        camSpin = !camSpin;
    }

    if (camSpin)
    {
        if (outOfPlace)
            resetState();
        float w = 40.0f * deltaTime;
        glm::mat4 rot = glm::mat3(1.0f);
        rot = glm::rotate(rot, glm::radians(w), glm::vec3(0, 1, 0));
        camera.Position = glm::vec3(rot * glm::vec4(camera.Position, 1.0f));
    }
    if (modelSpin)
    {
        float w = 40.0f * deltaTime;
        angle += w;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void moveModel(int dir, float deltaTime)
{
    float v = 2.50f * deltaTime;
    glm::vec3 Front = camera.Position;
    glm::vec3 Right = glm::normalize(glm::cross(Front, camera.WorldUp));
    glm::vec3 Up = glm::normalize(glm::cross(Right, Front));
    if (dir == UP)
    {
        pos += Up * v;
    }
    if (dir == DOWN)
    {
        pos -= Up * v;
    }
    if (dir == FORWARD)
    {
        pos -= Front * v;
    }
    if (dir == BACKWARD)
    {
        pos += Front * v;
    }
    if (dir == RIGHT)
    {
        pos -= Right * v;
    }
    if (dir == LEFT)
    {
        pos += Right * v;
    }
}

void resetState()
{
    outOfPlace = false;
    pos = glm::vec3(0, 0, 0);
    angle = 0;
    camera.Position = glm::vec3(0, 0, 3);
}