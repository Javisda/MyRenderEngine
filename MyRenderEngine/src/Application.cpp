#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "stb_image/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#include "Shader.h"
#include "Camera.h"
#include "Objects/Cube.h"
#include "TextureLoader.h"


using namespace std;

//VARIABLES
// Window
int windowWidth = 1280, windowHeight = 720;
// Camera
Camera camera;
// Time
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
// Camera mouse handling
float lastX = windowWidth/2, lastY = windowHeight/2; // Center of the window
bool firstMouse = true;

// Objects 
Cube cube;


// Funtion headers
void windowResize(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    // gL properties
    glViewport(0, 0, windowWidth, windowHeight); // Rendering window
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // This captures (stays within the center of the window) the cursor and hide it once the application has focus.

    // Creating the shaders
    Shader simpleTextureShader("src/Shaders/simpleTextureShader.vs", "src/Shaders/simpleTextureShader.fs");
    Shader simpleShader("src/Shaders/simpleShader.vs", "src/Shaders/simpleShader.fs");

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };


    // TEXTURE
    stbi_set_flip_vertically_on_load(true); // We assure all images are flipped correctly. (Nees to be call before any texture loading)
    
    // load and generate the texture
    TextureLoader* tex1 = new TextureLoader("src/Textures/wall.jpg");
    unsigned int texture1 = tex1->generateSimpleRGBTexture();

    TextureLoader* tex2 = new TextureLoader("src/Textures/goofy.png");
    unsigned int texture2 = tex2->generateSimpleRGBATexture();

    // Setting up the next, we make sure each uniform sampler correspond to the proper texture unit
    simpleTextureShader.use(); // don't forget to activate/use the shader before setting uniforms!
    simpleTextureShader.setInt("texture1", 0);
    simpleTextureShader.setInt("texture2", 1);


    // No lo vamos a usar de momento. VAO para simpleTextureShader
    /*
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube.verticesAndTexCoords), cube.verticesAndTexCoords, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    */

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube.verticesPositions), cube.verticesPositions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    

    // Camera
    glm::mat4 view;
    view = camera.GetViewMatrix();


    const float radius = 10.0f;



    // Transform matrices
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // note that we�re translating the scene in the reverse direction
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
    // Pass uniforms to the vertex shader
    simpleTextureShader.setMat4("view", view);
    simpleTextureShader.setMat4("model", model);
    simpleTextureShader.setMat4("proj", proj);

    simpleShader.use();
    simpleShader.setMat4("view", view);
    simpleShader.setMat4("model", model);
    simpleShader.setMat4("proj", proj);



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Time calculation
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Input
        processInput(window);
        // Mouse input
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        /* Rendering commands here */
        glfwSetFramebufferSizeCallback(window, windowResize); // Resize function

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // now render the triangle
        simpleTextureShader.use();
        simpleTextureShader.setColor("ourColor", 0.0f, 0.0f, 1.0f);

        // Camera
        proj = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        simpleTextureShader.setMat4("view", view);
        simpleTextureShader.setMat4("proj", proj);


        simpleShader.use();
        simpleShader.setMat4("view", view);
        simpleShader.setMat4("proj", proj);
        simpleShader.setColor("outColor", 1.0f, 0.0f, 0.0f);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            simpleTextureShader.setMat4("model", model);
            simpleShader.setMat4("model", model);
            simpleShader.setColor("outColor", 1.0f, (float)i/10.0, 0.0f);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        
        
        //glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
    simpleTextureShader.deleteProgram();

    glfwTerminate();
    return 0;
}

void windowResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    std::cout << "Screen info --> Width: " << width << ". Height: " << height << std::endl;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement calls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    
    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed: y ranges bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

