#include <glad.h>
#include <GLFW\glfw3.h>
#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <shader.h>
#include <vector>
#include <iostream>
using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* w);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void curse_poscallback(GLFWwindow *window, double x, double y);
void drawPoint(GLfloat x, GLfloat y, GLuint VAO, GLuint VBO);
void drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLuint VAO, GLuint VBO);
void drawBezier(GLuint VAO, GLuint VBO);
GLfloat vertices[] = {
	0.0f, 0.0f, 0.0f
};
GLfloat vertices2[] = {
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f
};

vector<pair<GLfloat,GLfloat>> points;
int cnt = 0;
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* w = glfwCreateWindow(800, 800, "triangle", NULL, NULL);
	if (w == NULL) {
		cout << "Fail to create GLFW windows" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(w);
	glfwSetFramebufferSizeCallback(w, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Fail to initialize GLAD" << endl;
		return -1;
	}

	glViewport(0, 0, 800, 800);
	Shader shader("vertex.vs", "fragment.fs");

	int mode = 0;
	GLuint VBO, VAO, VAO2, VBO2;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(w, true);
	ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	while (!glfwWindowShouldClose(w)) {
		processInput(w);
		glm::vec3 lightPos = glm::vec3(sin(glfwGetTime())*3, 5.0f, cos(glfwGetTime())*3);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::SetWindowSize(ImVec2(260, 95));
		glfwSetMouseButtonCallback(w, mouse_button_callback);
		glViewport(0, 0, 800, 800);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		for (int i = 0; i < points.size(); i++) {
			drawPoint(points[i].first, points[i].second, VAO, VBO);
		}
		if (points.size() > 1) {
			for (int i = 0; i < points.size()- 1; i++) {
				drawLine(points[i].first, points[i].second, points[i+1].first, points[i+1].second, VAO2, VBO2);
			}
		}
		if (points.size() == 4) {
			drawBezier(VAO, VBO);
		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(w);
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}
void drawPoint(GLfloat x, GLfloat y, GLuint VAO, GLuint VBO) {
	vertices[0] = x;
	vertices[1] = y;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);
}
void drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLuint VAO2, GLuint VBO2) {
	vertices2[0] = x1;
	vertices2[1] = y1;
	vertices2[3] = x2;
	vertices2[4] = y2;
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glBindVertexArray(VAO2);
	glDrawArrays(GL_LINES, 0, 2);
}

void drawBezier(GLuint VAO, GLuint VBO) {
	GLfloat x = 0;
	GLfloat y = 0;
	for (float t = 0; t < 1 + 0.5 / 1000; t += 1 / float(1000)) {
		x = points[0].first*pow(1.0f - t, 3) + 3 * points[1].first*t*pow(1.0f - t, 2) + 3 * points[2].first*t*t*(1.0f - t) + points[3].first*pow(t, 3);
		y = points[0].second*pow(1.0f - t, 3) + 3 * points[1].second*t*pow(1.0f - t, 2) + 3 * points[2].second*t*t*(1.0f - t) + points[3].second*pow(t, 3);
		drawPoint(x, y, VAO, VBO);
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* w) {
	if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(w, true);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		cnt += 1;
		glfwSetCursorPosCallback(window, curse_poscallback);
	}
	return;
}
void curse_poscallback(GLFWwindow *window, double x, double y) {
	if (points.size() < cnt&&points.size() < 4) {
		points.push_back(make_pair((x-400)/400, (400 - y) / 400));
	}
	else if (points.size() == 4&&cnt%4 != 0) {
		points[cnt % 4 - 1] = make_pair((x - 400) / 400, (400 - y) / 400);
	}
	else if (points.size() == 4 && cnt % 4 == 0&&cnt > 4) {
		points[3] = make_pair((x - 400) / 400, (400 - y) / 400);
	}
}
