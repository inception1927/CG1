//
//  main.cpp
//  hw8
//
//  Created by 陶文宇 on 2018/3/19.
//  Copyright © 2018年 陶文宇. All rights reserved.
//

#include <iostream>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <math.h>
#include "shader(1).h"

using std::cout;
using std::endl;

int nearestIndex = -1;
bool isPressed = false;

int pointNumber = 0;
bool isSet[] = {false, false, false, false};
//int pointNumber = 4;
//bool isSet[] = {true,true,true,true};
int pixelNumber = 1000;

float vertices[] = {
    0.0f, 0.0f, 0.0f
};

float vertices2[] = {
    -0.5f, -0.2f, 0.0f,
    -0.2f, 0.1f, 0.0f,
    0.3f, 0.2f, 0.0f,
    0.4f, -0.2f, 0.0f
};

float vertices3[] = {
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f
};

unsigned int VAO, VBO, VAO2, VBO2;

glm::vec4 color(0.0f, 0.0f, 0.0f, 0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window,int button, int state, int mods);
void drawLine(GLFWwindow* window, unsigned int VAO, unsigned int VBO);
float getFac(int num);


void Bresenham_line(int x0, int y0, int x1, int y1, unsigned int VAO, unsigned int VBO);
void drawpoint(float x, float y, unsigned int VAO, unsigned int VBO);
void drawStrLine(float x1, float y1, float x2, float y2, unsigned int VAO,unsigned int VBO);





int NearestPoint(float x, float y);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hw8", NULL, NULL);
    if (window == NULL) {
        cout << "Fail to create GLFW windows" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Fail to initialize GLAD" << endl;
        return -1;
    }
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    
    glViewport(0, 0, screenWidth, screenHeight);
    
    Shader shaderProgream(vertexShaderSource,fragmentShaderSource);
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    
    
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    

    int mode = 0;
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);
    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    
    while (!glfwWindowShouldClose(window)) {
        
        processInput(window);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        //create imgui window
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();
        ImGui::SetWindowSize(ImVec2(220, 300));
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        ImGui::ColorPicker3("Line Color", (float*)&color);
        
        if (mode == 0) {
            
            shaderProgream.use();
            shaderProgream.setVec4("Color", color);
            glBindVertexArray(VAO);
            drawLine(window, VAO, VBO);
//            drawStrLine(-0.5f, 0.1f, 0.3f, 0.2f, VAO2, VBO2);

        }

        if (ImGui::Button("Line")) {
            mode = 0;
        }

        
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    
    return 0;
}

void drawLine(GLFWwindow* window, unsigned int VAO, unsigned int VBO) {
    if (isPressed && nearestIndex != -1) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        x = (x - width / 2) / width * 2;
        y = 0 - (y - height / 2) / height * 2;
        vertices2[nearestIndex * 3] = x;
        vertices2[nearestIndex * 3 + 1] = y;
    }
    for (int i = 0; i < 4; i++) {
        if (isSet[i]) {
            vertices[0] = vertices2[i * 3];
            vertices[1] = vertices2[i * 3 + 1];
            glPointSize(5);
            drawpoint(vertices[0], vertices[1], VAO, VBO);
        }
    }
    if (pointNumber > 1) {
        for (int j = 0; j < pointNumber -1 ; j++){
            drawStrLine(vertices2[j*3], vertices2[j*3+1], vertices2[(j+1)*3], vertices2[(j+1)*3+1], VAO2, VBO2);
        }
    }
    if (pointNumber < 4) {
        return;
    }
    int n = 3;
    for (float i = 0; i < 1 + 0.5 / pixelNumber; i += 1 / float(pixelNumber)) {
        float pointX_t = 0;
        float pointY_t = 0;
        for (int j = 0; j <= n; j++) {
            float x = vertices2[j * 3];
            float y = vertices2[j * 3 + 1];
            float B = getFac(n) / (getFac(j) * getFac(n - j)) * pow(i, j) * pow((1 - i), n - j);
            pointX_t += x * B;
            pointY_t += y * B;
        }
        glPointSize(2);
        drawpoint(pointX_t, pointY_t, VAO, VBO);
    }
}

void drawStrLine(float x0, float y0, float x1, float y1, unsigned int VAO2, unsigned int VBO2) {
    vertices3[0] = x0; vertices3[1] = y0;
    vertices3[3] = x1; vertices3[4] = y1;
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);
    glBindVertexArray(VAO2);
    glDrawArrays(GL_LINES, 0, 2);
    
}

void drawpoint(float x, float y, unsigned int VAO, unsigned int VBO) {
    vertices[0] = x; vertices[1] = y;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 1);
}



void mouse_button_callback(GLFWwindow* window,int button, int state, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    x = (x  - SCR_WIDTH / 2) / SCR_WIDTH * 2;
    y = 0 - (y - SCR_HEIGHT / 2) / SCR_HEIGHT * 2;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (GLFW_PRESS == state) {
            if (pointNumber < 4) {
                for (int i = 0; i < 4; i++) {
                    if (!isSet[i]) {
                        vertices2[i * 3] = float(x);
                        vertices2[i * 3 + 1] = float(y);
                        isSet[i] = true;
                        break;
                    }
                }
                pointNumber++;
                return;
            }
            isPressed = true;
            nearestIndex = NearestPoint(x, y);
        }
        if (GLFW_RELEASE == state) {
            isPressed = false;
            nearestIndex = -1;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        int nearest = NearestPoint(x, y);
        if (nearest != -1) {
            isSet[nearest] = false;
            pointNumber--;
        }
    }
}

float getFac(int num) {
    if (0 == num) return 1.0f;
    if (1 == num) return 1.0f;
    if (2 == num) return 2.0f;
    else return 6.0f;
}

int NearestPoint(float x, float y) {
    float dis = 10;
    int min = -1;
    int ret = -1;
    x *= 1000;
    y *= 1000;
    for (int i = 0; i < 4; i++) {
        if (isSet[i]) {
            float x_ = vertices2[i * 3] * 1000;
            float y_ = vertices2[i * 3 + 1] * 1000;
            float tmp = sqrt(pow(y_ - y, 2) + pow(x_ - x, 2));
            if (tmp <= dis && (min == -1 || tmp < min)) {
                ret = i;
            }
        }
    }
    return ret;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glfwSetWindowSize(window, width, height);
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


