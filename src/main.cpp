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
    glEnable(GL_BLEND);
    // Enable depth
    // glEnable(GL_DEPTH_TEST);

    // Compiler shaders
    Shader ourShader("../src/vertex.shader", "../src/fragment.shader");

    // Vertices
    float vertices[4 * pn][9];
    for (int i = 0; i < pn; i++)
    {
        vertices[i][0] = 0;
        vertices[i][1] = 0;
        vertices[i][2] = prismLen;
        vertices[i][3] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][4] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][5] = prismLen;
        vertices[i][6] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][7] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][8] = prismLen;
    }
    for (int i = pn; i < 2 * pn; i++)
    {
        vertices[i][0] = 0;
        vertices[i][1] = 0;
        vertices[i][2] = -prismLen;
        vertices[i][3] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][4] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][5] = -prismLen;
        vertices[i][6] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][7] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][8] = -prismLen;
    }
    for (int i = 2 * pn; i < 3 * pn; i++)
    {
        vertices[i][0] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][1] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][2] = prismLen;
        vertices[i][3] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][4] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][5] = -prismLen;
        vertices[i][6] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][7] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][8] = -prismLen;
    }
    for (int i = 3 * pn; i < 4 * pn; i++)
    {
        vertices[i][0] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][1] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][2] = -prismLen;
        vertices[i][3] = 0.5 * cos(i * 2 * M_PI / pn);
        vertices[i][4] = 0.5 * sin(i * 2 * M_PI / pn);
        vertices[i][5] = prismLen;
        vertices[i][6] = 0.5 * cos((i + 1) * 2 * M_PI / pn);
        vertices[i][7] = 0.5 * sin((i + 1) * 2 * M_PI / pn);
        vertices[i][8] = prismLen;
    }

    // Gpu buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);                                                         // Init VAO
    glGenBuffers(1, &VBO);                                                              // Generates vertex buffers
    glBindVertexArray(VAO);                                                             // Bind VBO, VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                                 // Binds buffers to VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (float *)vertices, GL_STATIC_DRAW); // Uploads data to GPU

    // Link vertex array
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Tells openGL how to interpret the data
    glEnableVertexAttribArray(0);                                                  // Enables vertex attribute array

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
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}