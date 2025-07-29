#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <numeric>
#define GRAVITATIONAL_CONSTANT 0.1f

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int SRC_HEIGHT = 480;
int SRC_WIDTH = 640;
int NUMCIRCLES = 2; // Number of circles to simulate


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
    

    GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Gravity Simulation", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  

    // sets the viewport to the size of the window
    glViewport(0, 0, SRC_WIDTH, SRC_HEIGHT);

    // resizes the viewport when the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

    // Generate circle vertices
    const int segments = 32;
    std::vector<float> circleVertices;
    
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

    // Generate indices for triangle fan
    std::vector<unsigned int> indices;
    for (int i = 1; i <= segments; i++) {
        indices.push_back(0);      // center
        indices.push_back(i);      // current vertex
        indices.push_back(i + 1);  // next vertex
    }
    // Close the circle
    indices[indices.size() - 1] = 1;

    // Separate dynamic and static instance data
    std::vector<float> basePositions;     // Relative positions (dynamic)
    std::vector<float> positions;         // Actual positions (updated each frame)
    std::vector<float> radiusColorData;   // Radius + Color (static)
    const float radius = 0.05f;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-0.3f, 0.3f);  // Relative to group center
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
    
    for (int i = 0; i < NUMCIRCLES; i++) {
        // Base relative positions (2 floats per circle)
        basePositions.push_back(posDist(gen));
        basePositions.push_back(posDist(gen));
        
        // Initialize actual positions
        positions.push_back(basePositions[i * 2]);
        positions.push_back(basePositions[i * 2 + 1]);
        
        // Static radius + color data (4 floats per circle)
        radiusColorData.push_back(radius);                // Radius
        radiusColorData.push_back(colorDist(gen));       // R
        radiusColorData.push_back(colorDist(gen));       // G
        radiusColorData.push_back(colorDist(gen));       // B
    }

    // Create buffers
    unsigned int VBO, VAO, EBO, positionVBO, radiusColorVBO;
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

    // creating a vertex shader
    unsigned int vertexShader;
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

    // creating a fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // attaching the source code to the fragment shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // creating a shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Unbind VAO
    glBindVertexArray(0);


    std::vector<float> velocity(NUMCIRCLES * 2, 0.0f);
    std::vector<int> mass(NUMCIRCLES, 1);
    std::vector<int> gravityX(NUMCIRCLES, 0);
    std::vector<int> gravityY(NUMCIRCLES, 0);

    float sumGravityX, sumGravityY;
    float deltaTime = 0.0f;
    float lastTime = 0.0f;
    float currentTime;
    float dx, dy;  // Changed from int to float for proper calculations
    float distance;
    float angle;
    
    // Pre-declare variables used in loops for better performance
    float distanceSquared, invDistance, nx, ny, overlap;
    float separationX, separationY, relativeVelX, relativeVelY, velAlongNormal, impulse;
    float gravityDeltaX, gravityDeltaY;
    
    // Pre-calculate constants for optimization
    const float radiusSum = 2.0f * radius;
    const float radiusSumSquared = radiusSum * radiusSum;
    const float wallLeft = -1.0f + radius;
    const float wallRight = 1.0f - radius;
    const float wallBottom = -1.0f + radius;
    const float wallTop = 1.0f - radius;
    
    // Initialize with some random velocities
    /*std::uniform_real_distribution<float> velDist(-2.0f, 2.0f);
    for (int i = 0; i < NUMCIRCLES; i++) {
        velocity[i * 2] = velDist(gen);     // Random X velocity
        velocity[i * 2 + 1] = velDist(gen); // Random Y velocity
    }*/

    std::uniform_int_distribution<int> massDist(1, 2);
    for (int i = 0; i < NUMCIRCLES; i++) {
        mass[i] = massDist(gen); // Random mass between 1 and 10
    }


    // render loop
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
 
        // Circle-to-circle collision detection (optimized)
        for (int i = 0; i < NUMCIRCLES; i++) {
            for(int j = i + 1; j < NUMCIRCLES; j++) { // Only check each pair once
                dx = positions[i * 2] - positions[j * 2];
                dy = positions[i * 2 + 1] - positions[j * 2 + 1];
                
                // Quick distance check using squared distance (avoid sqrt)
                distanceSquared = dx * dx + dy * dy;
                
                if(i != j && distanceSquared > 0.0001f ){
                    angle = atan2(dy, dx);
                    printf("angle: %f\n", angle);
                    printf("Distance Squared: %f\n", distanceSquared);
                    gravityX[j] = (mass[j]/distanceSquared) * GRAVITATIONAL_CONSTANT * cos(angle);
                    gravityY[j] = (mass[j]/distanceSquared) * GRAVITATIONAL_CONSTANT * sin(angle);
                }
                
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
                    impulse = velAlongNormal;
                    
                    velocity[i * 2] -= impulse * nx;
                    velocity[i * 2 + 1] -= impulse * ny;
                    velocity[j * 2] += impulse * nx;
                    velocity[j * 2 + 1] += impulse * ny;
                }
            }
        }
        

        for (int i = 0; i < NUMCIRCLES; i++) {
            sumGravityX = std::accumulate(gravityX.begin(), gravityX.end(), 0.0f);
            sumGravityY = std::accumulate(gravityY.begin(), gravityY.end(), 0.0f);

            gravityDeltaY = sumGravityY * deltaTime;
            gravityDeltaX = sumGravityX * deltaTime;

            // Apply gravity effect (only to Y velocity)
            velocity[i * 2 + 1] -= gravityDeltaY;
            velocity[i * 2] -= gravityDeltaX;
            
            // Update positions based on velocity
            positions[i * 2] += velocity[i * 2] * deltaTime;         // x position
            positions[i * 2 + 1] += velocity[i * 2 + 1] * deltaTime; // y position
            
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
        
        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), positions.data());
        
        // clearing the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader program
        glUseProgram(shaderProgram);
        
        // Bind the VAO and draw all instances
        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, NUMCIRCLES);

        // process input from keyboard
        processInput(window);

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