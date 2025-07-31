#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <numeric>
#include <chrono>
#include <unistd.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void generatePositionsAndStaticData(std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>& );
void genAndBindBuffers(unsigned int&, unsigned int&, std::vector<float>&, std::vector<float>&, std::vector<unsigned int>&, std::vector<float>&);
int initWindow(GLFWwindow *window);
void creatingCircles(std::vector<float>&, std::vector<unsigned int>&);
int creatingVertexShader(unsigned int);
int creatingFragmentShader(unsigned int);
int creatingShaderProgram(unsigned int, unsigned int, unsigned int);

int SRC_HEIGHT = 640;
int SRC_WIDTH = 640;
const int NUMCIRCLES = 5; // Number of circles to simulate
const float radius = 0.025f;

// Frame rate limiting variables
const float TARGET_FPS = 60.0f;
const float UPDATER_PER_FRAME = 8.0f;
const float deltaTime = (1.0f / TARGET_FPS) / UPDATER_PER_FRAME;



const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 instancePos;\n"
    "layout (location = 2) in float instanceRadius;\n"
    "layout (location = 3) in vec3 instanceColor;\n"
    "out vec3 fragColor;\n"
    "void main()\n"
    "{\n"
    "   vec3 worldPos = aPos * instanceRadius + vec3(instancePos, 0.0);\n"
    "   gl_Position = vec4(worldPos, 1.0);\n"
    "   fragColor = instanceColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "in vec3 fragColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(fragColor, 1.0f);\n"
    "}\0";


int main(void) {
    
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "FPS: -", NULL, NULL);
    initWindow(window);
    
    std::vector<float> circleVertices, positions, lastPositions, radiusColorData;
    std::vector<float> velocity(NUMCIRCLES * 2, 0.0f), acceleration(NUMCIRCLES * 2, 0.0f);

    unsigned int VAO, positionVBO, vertexShader, fragmentShader, shaderProgram;

    positions.clear();
    radiusColorData.clear();
    std::vector<unsigned int> indices;
    
    // generating circle by composing them of smaller triangles
    creatingCircles(circleVertices, indices);

    // generate position of first circle and static data for instances
    generatePositionsAndStaticData(lastPositions, positions, radiusColorData, acceleration);
    
    genAndBindBuffers(VAO, positionVBO, positions, radiusColorData, indices, circleVertices);

    vertexShader = creatingVertexShader(vertexShader);

    fragmentShader = creatingFragmentShader(fragmentShader);

    shaderProgram = creatingShaderProgram(shaderProgram, fragmentShader, vertexShader);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Unbind VAO
    glBindVertexArray(0);

    
    // // Pre-calculate constants for optimization
    const float radiusSum = 2.0f * radius;
    const float radiusSumSquared = radiusSum * radiusSum;
    const float wallLeft = -1.0f + radius;
    const float wallRight = 1.0f - radius;
    const float wallBottom = -1.0f + radius;
    const float wallTop = 1.0f - radius;

    // spawning circles
    int remainingCirclesToSpawn = NUMCIRCLES - 1;

    // FPS check
    int frames = 0;
    bool reset = false;
    
    auto frameStartTime = std::chrono::steady_clock::now();
    auto lastTime = frameStartTime;
    std::chrono::duration<float> deltaTimeDuration;
    auto timer = frameStartTime;
    float actualDeltaTime = 0.0f;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        frameStartTime = std::chrono::steady_clock::now();
        deltaTimeDuration = frameStartTime - lastTime;
        lastTime = frameStartTime;
        actualDeltaTime = deltaTimeDuration.count();


        // spawn a circle (if they are not over) every 0.25 seconds
        auto timeFromLastSpawn = frameStartTime - timer;
        if(remainingCirclesToSpawn > 0 && timeFromLastSpawn >= std::chrono::nanoseconds(250000000)){
            generatePositionsAndStaticData(lastPositions, positions, radiusColorData, acceleration);
            remainingCirclesToSpawn--;
        }

        // reset the timer for the frame counter
        if(reset){
            timer = lastTime;
            reset = false;
        }

        
        // wall collisions
        for (int i = 0; i < NUMCIRCLES - remainingCirclesToSpawn; i++) {
            
            // Bounce off left and right walls (using pre-calculated constants)
            if(positions[i * 2] <= wallLeft) {
                positions[i * 2] = wallLeft;

                velocity[i * 2] = -velocity[i * 2]; // Simply reverse X velocity
            }
            else if(positions[i * 2] >= wallRight) {
                positions[i * 2] = wallRight;

                velocity[i * 2] = -velocity[i * 2]; // Simply reverse X velocity
            }
            
            // Bounce off top and bottom walls (using pre-calculated constants)
            if(positions[i * 2 + 1] <= wallBottom) {
                positions[i * 2 + 1] = wallBottom;

                velocity[i * 2 + 1] = -velocity[i * 2 + 1]; // Simply reverse Y velocity
            }
            else if(positions[i * 2 + 1] >= wallTop) {
                positions[i * 2 + 1] = wallTop;

                velocity[i * 2 + 1] = -velocity[i * 2 + 1]; // Simply reverse Y velocity
            }
        }
        
       // Update positions based on Verlet integration
        for (int i = 0; i < NUMCIRCLES - remainingCirclesToSpawn; i++){
            // Store current position as next frame's lastPosition
            float tempX = positions[i * 2];
            float tempY = positions[i * 2 + 1];
            
            // Verlet integration: x(n+1) = 2*x(n) - x(n-1) + a*dt^2
            positions[i * 2] = 2.0f * positions[i * 2] - lastPositions[i * 2] + acceleration[i * 2] * deltaTime * deltaTime;
            positions[i * 2 + 1] = 2.0f * positions[i * 2 + 1] - lastPositions[i * 2 + 1] + acceleration[i * 2 + 1] * deltaTime * deltaTime;
            
            // Update lastPositions for next frame
            lastPositions[i * 2] = tempX;
            lastPositions[i * 2 + 1] = tempY;
        }

        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), positions.data());
        
        // clearing the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader program
        glUseProgram(shaderProgram);
        
        // Bind the VAO and draw all instances
        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, NUMCIRCLES - remainingCirclesToSpawn);

        // process input from keyboard
        processInput(window);

        
        // frameTime = glfwGetTime() - frameStartTime;
        // if (frameTime < TARGET_FRAME_TIME) {
        //     float remainingTime = TARGET_FRAME_TIME - frameTime;
        //     int sleepMicroseconds = (int)(remainingTime * 1000000.0f);
        //     if (sleepMicroseconds > 0) {
        //        usleep(sleepMicroseconds);
        //     }
        // }

        frames++;
        if(std::chrono::steady_clock::now() - timer > std::chrono::seconds(1)){
            std::string title = "FPS: " + std::to_string(static_cast<int>(frames));
            glfwSetWindowTitle(window, title.c_str());
            reset = true;
            frames = 0;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	SRC_WIDTH = newWidth;
	SRC_HEIGHT = newHeight;
	//std::cout << "New resolution: " << SRC_WIDTH << "x" << SRC_HEIGHT << std::endl;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void generatePositionsAndStaticData(std::vector<float>& lastPositions, std::vector<float>& positions, std::vector<float>& radiusColorData, std::vector<float>& acceleration) {

    positions.push_back(-0.9f); // X position
    positions.push_back(0.9f); // Y position
    lastPositions.push_back(-0.9f); // X position
    lastPositions.push_back(0.9f);

    acceleration.push_back(0.0f);   // 0 m/s^2 on X
    acceleration.push_back(-9.81f);  // gravity on Y

    radiusColorData.push_back(radius);
    radiusColorData.push_back(1.0f);
    radiusColorData.push_back(1.0f);
    radiusColorData.push_back(1.0f);

    float velocityX = 0.3f; // Initial velocity
    float velocityY = -0.3f; // Initial velocity

    // setting up velocity this way we use the formula (xn - x(n-1))/deltaT = v
    int currentCircleIndex = (positions.size() / 2) - 1; // Get the index of the circle we just added
    lastPositions[currentCircleIndex * 2] = positions[currentCircleIndex * 2] - velocityX * deltaTime;
    lastPositions[currentCircleIndex * 2 + 1] = positions[currentCircleIndex * 2 + 1] - velocityY * deltaTime;
}

void genAndBindBuffers(unsigned int& VAO, unsigned int& positionVBO, std::vector<float>& positions, std::vector<float>& radiusColorData, std::vector<unsigned int>& indices, std::vector<float>& circleVertices){
    unsigned int VBO,  EBO, radiusColorVBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &positionVBO);
    glGenBuffers(1, &radiusColorVBO);

    glBindVertexArray(VAO);

    // Bind and fill vertex buffer with circle geometry
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);

    // Bind and fill element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set vertex attributes for circle geometry
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Bind and fill instance buffer for positions (dynamic data)
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);

    // Set position attributes (location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // This makes it instanced

    // Bind and fill instance buffer for radius and color (static data)
    glBindBuffer(GL_ARRAY_BUFFER, radiusColorVBO);
    glBufferData(GL_ARRAY_BUFFER, radiusColorData.size() * sizeof(float), radiusColorData.data(), GL_STATIC_DRAW);

    // Set radius attributes (location 2)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1); // This makes it instanced

    // Set color attributes (location 3)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(1 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1); // This makes it instanced
}

int initWindow(GLFWwindow *window){

    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    
    // Enable V-Sync to limit to monitor refresh rate (usually 60 FPS)
    glfwSwapInterval(0);  // 1 = enable V-Sync, 0 = disable

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  

    // sets the viewport to the size of the window
    glViewport(0, 0, SRC_WIDTH, SRC_HEIGHT);

    // resizes the viewport when the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // RGBA values
    return 0;
}

void creatingCircles(std::vector<float>& circleVertices, std::vector<unsigned int>& indices){
    const int segments = 16;

    // Center vertex
    circleVertices.push_back(0.0f);
    circleVertices.push_back(0.0f);
    circleVertices.push_back(0.0f);
    
    // Generate vertices around the circle
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        circleVertices.push_back(cos(angle));
        circleVertices.push_back(sin(angle));
        circleVertices.push_back(0.0f);
    }

    for (int i = 1; i <= segments; i++) {
        indices.push_back(0);      // center
        indices.push_back(i);      // current vertex
        indices.push_back(i + 1);  // next vertex
    }
    // Close the circle
    indices[indices.size() - 1] = 1;

}

int creatingVertexShader(unsigned int vertexShader){
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // attaching the source code to the vertex shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return vertexShader;
}

int creatingFragmentShader(unsigned int fragmentShader){
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // attaching the source code to the fragment shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return fragmentShader;
}

int creatingShaderProgram(unsigned int shaderProgram, unsigned int fragmentShader, unsigned int vertexShader){
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    return shaderProgram;
}

/*        // Circle-to-circle collision detection (optimized)
        for (int i = 0; i < NUMCIRCLES; i++) {
            for(int j = i + 1; j < NUMCIRCLES; j++) { // Only check each pair once
                dx = positions[i * 2] - positions[j * 2];
                dy = positions[i * 2 + 1] - positions[j * 2 + 1];
                
                // Quick distance check using squared distance (avoid sqrt)
                distanceSquared = dx * dx + dy * dy;
                
                // Check if circles are colliding using squared distance
                if(distanceSquared < radiusSumSquared && distanceSquared > 0.0001f) { // Avoid division by zero
                    // Only calculate sqrt when we know there's a collision
                    distance = sqrt(distanceSquared);
                    
                    // Normalize the collision vector
                    invDistance = 1.0f / distance;  // Cache inverse
                    nx = dx * invDistance;  // Normal X component
                    ny = dy * invDistance;  // Normal Y component
                    
                    // Separate the circles to prevent overlap
                    overlap = radiusSum - distance;
                    separationX = nx * overlap * 0.5f;
                    separationY = ny * overlap * 0.5f;
                    
                    positions[i * 2] += separationX;
                    positions[i * 2 + 1] += separationY;
                    positions[j * 2] -= separationX;
                    positions[j * 2 + 1] -= separationY;
                    
                    // Calculate relative velocity
                    relativeVelX = velocity[i * 2] - velocity[j * 2];
                    relativeVelY = velocity[i * 2 + 1] - velocity[j * 2 + 1];
                    
                    // Calculate relative velocity in collision normal direction
                    velAlongNormal = relativeVelX * nx + relativeVelY * ny;
                    
                    // Do not resolve if velocities are separating
                    if(velAlongNormal > 0) continue;
                    
                    // Apply elastic collision response (equal mass assumption)
                    impulse = 2.0f * velAlongNormal / (mass[i] + mass[j]);
                    
                    velocity[i * 2] -= impulse * nx;
                    velocity[i * 2 + 1] -= impulse * ny;
                    velocity[j * 2] += impulse * nx;
                    velocity[j * 2 + 1] += impulse * ny;
                }
            }
        }*/