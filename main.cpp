#define STB_IMAGE_IMPLEMENTATION

#include <array>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm/ext.hpp>
#include <stb_image.h>
#include <GLFW\glfw3.h>

int WIDTH = 800;
int HEIGHT = 600;

std::string readFile(const char* filePath) {
	std::string fileContents;
	if (std::ifstream fileStream{ filePath, std::ios::in })
		fileContents.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());

	return fileContents;
}

void checkShader(GLuint shaderId) {
	GLint result = GL_TRUE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
		GLint logLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0) {
			std::string shaderLog(logLength, '\0');
			glGetShaderInfoLog(shaderId, logLength, nullptr, &shaderLog[0]);
			std::cout << "Erro no shader" << std::endl;
			std::cout << shaderLog << std::endl;

			assert(false);
		}
	}
}

GLuint loadShaders(const char* vertexShaderFile, const char* fragmentShaderFile) {
	std::string vertexShaderSource = readFile(vertexShaderFile);
	std::string fragmentShaderSource = readFile(fragmentShaderFile);

	assert(!vertexShaderSource.empty());
	assert(!fragmentShaderSource.empty());

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	std::cout << "Loading shader: " << vertexShaderFile << std::endl;

	const char* vertexShaderSourcePtr = vertexShaderSource.c_str();
	glShaderSource(vertexShaderId, 1, &vertexShaderSourcePtr, nullptr);
	glCompileShader(vertexShaderId);
	checkShader(vertexShaderId);

	std::cout << "Loading shader: " << fragmentShaderFile << std::endl;

	const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSourcePtr, nullptr);
	glCompileShader(fragmentShaderId);
	checkShader(fragmentShaderId);

	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	GLint result = GL_TRUE;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);

	if (result == GL_FALSE) {
		GLint logLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0) {
			std::string programLog(logLength, '\0');
			glGetProgramInfoLog(programId, logLength, nullptr, &programLog[0]);

			std::cout << "Error ao linkar o programa" << std::endl;
			std::cout << programLog << std::endl;
			assert(false);
		}
	}

	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	return programId;
}

GLuint loadTexture(const char* textureFile) {
	std::cout << "Loading texture: " << textureFile << std::endl;

	stbi_set_flip_vertically_on_load(true);

	int width = 0;
	int height = 0;
	int componentsAmount = 0;
	unsigned char* texture = stbi_load(textureFile, &width, &height, &componentsAmount, 3);

	assert(texture);

	GLuint textureId = 0;
	glGenTextures(1, &textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texture);

	return textureId;
}

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
};

struct DirectionalLight {
	glm::vec3 direction;
	GLfloat intensity;
};

GLuint loadGeometry() {
	std::array<Vertex, 4> square = {
		Vertex{ glm::vec3{ -1.0f, -1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f } },
		Vertex{ glm::vec3{  1.0f, -1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f } },
		Vertex{ glm::vec3{  1.0f,  1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f } },
		Vertex{ glm::vec3{ -1.0f,  1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 1.0f } }
	};

	std::array<glm::ivec3, 2> indexes = {
		glm::ivec3{ 0, 1 ,3 },
		glm::ivec3{ 3, 1 ,2 }
	};

	GLuint VertexBuffer = 0;
	GLuint IndexBuffer = 0;
	glGenBuffers(1, &VertexBuffer);
	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes.data(), GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

	glBindVertexArray(0);

	return vao;
}

void generateSphereMesh(GLuint resolution, std::vector<Vertex>& vertices, std::vector<glm::ivec3>& indices) {
	vertices.clear();
	indices.clear();

	constexpr float pi = glm::pi<float>();
	constexpr float two_pi = glm::two_pi<float>();
	const float inv_resolution = 1.0f / static_cast<float>(resolution - 1);

	for (GLuint u_i = 0; u_i < resolution; ++u_i) {
		const float u = u_i * inv_resolution;
		const float phi = two_pi * u;

		for (GLuint v_i = 0; v_i < resolution; ++v_i) {
			const float v = v_i * inv_resolution;
			const float theta = pi * v;

			glm::vec3 vertex_position{
				glm::sin(theta) * glm::cos(phi),
				glm::sin(theta) * glm::sin(phi),
				glm::cos(theta)
			};

			Vertex vertex{
				vertex_position,
				glm::normalize(vertex_position),
				glm::vec3{ 1.0f, 1.0f, 1.0f },
				glm::vec2{ 1.0f - u, v }
			};

			vertices.push_back(vertex);

			if (u_i < resolution - 1 && v_i < resolution - 1) {
				GLuint p0 = u_i + v_i * resolution;
				GLuint p1 = (u_i + 1) + v_i * resolution;
				GLuint p2 = (u_i + 1) + (v_i + 1) * resolution;
				GLuint p3 = u_i + (v_i + 1) * resolution;

				indices.push_back(glm::ivec3{ p0, p1, p3 });
				indices.push_back(glm::ivec3{ p3, p1, p2 });
			}
		}
	}
}

GLuint loadSphere(GLuint& num_vertices, GLuint& num_indices) {
	std::vector<Vertex> vertices;
	std::vector<glm::ivec3> indices;
	generateSphereMesh(180, vertices, indices);

	num_vertices = vertices.size();
	num_indices = indices.size() * 3;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	GLuint element_buffer;
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

	glBindVertexArray(0);

	return vao;
}

class FlyCamera {
public:
	glm::vec3 location{ 0.0f, 0.0f , 10.0f };
	glm::vec3 direction{ 0.0f, 0.0f , -1.0f };
	glm::vec3 up{ 0.0f, 1.0f , 0.0f };

	float fov = glm::radians(45.0f);
	float aspectRadtio = static_cast<float>(WIDTH) / HEIGHT;
	float near = 0.01f;
	float far = 1000.0f;

	float speed = 6.0f;
	float sensitivity = 0.1f;

	glm::mat4 getView() const {
		return glm::lookAt(location, location + direction, up);
	}

	glm::mat4 getViewProjection() const {
		glm::mat4 projection = glm::perspective(fov, aspectRadtio, near, far);
		return projection * getView();
	}

	void moveForward(float amount) {
		location += glm::normalize(direction) * amount * speed;
	}

	void moveRight(float amount) {
		glm::vec3 right = glm::normalize(glm::cross(direction, up));
		location += right * amount * speed;
	}

	void look(float yaw, float pitch) {
		yaw *= sensitivity;
		pitch *= sensitivity;

		const glm::vec3 right = glm::normalize(glm::cross(direction, up));
		const glm::mat4 i = glm::identity<glm::mat4>();

		glm::mat4 yawRotation = glm::rotate(i, glm::radians(yaw), up);
		glm::mat4 pitchRotation = glm::rotate(i, glm::radians(pitch), right);

		up = pitchRotation * glm::vec4{ up, 0.0f };
		direction = yawRotation * pitchRotation * glm::vec4{ direction, 0.0f };
	}
};

FlyCamera Camera;

bool enableMouseMovement = false;
glm::vec2 previousCursor{ 0.0, 0.0 };

void mouseButtomCb(GLFWwindow* Window, int buttom, int action, int modifiers) {
	if (buttom == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			double x = 0, y = 0;
			glfwGetCursorPos(Window, &x, &y);
			previousCursor = glm::vec2{ x, y };
			enableMouseMovement = true;
		}
		else {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			enableMouseMovement = false;
		}
	}
}

void mouseMotionCb(GLFWwindow* Window, double x, double y) {
	if (enableMouseMovement) {
		glm::vec2 currentCursor{ x, y };
		glm::vec2 deltaCursor = currentCursor - previousCursor;

		Camera.look(-deltaCursor.x, -deltaCursor.y);

		previousCursor = currentCursor;
	}
}

void resize(GLFWwindow* Window, int new_width, int new_height) {
	WIDTH = new_width;
	HEIGHT = new_height;

	Camera.aspectRadtio = static_cast<float>(WIDTH) / HEIGHT;
	glViewport(0, 0, WIDTH, HEIGHT);
}

int main() {
	assert(glfwInit() == GLFW_TRUE);

	GLFWwindow* Window = glfwCreateWindow(WIDTH, HEIGHT, "Blue Marble", nullptr, nullptr);
	assert(Window);

	glfwSetMouseButtonCallback(Window, mouseButtomCb);
	glfwSetCursorPosCallback(Window, mouseMotionCb);
	glfwSetFramebufferSizeCallback(Window, resize);

	glfwMakeContextCurrent(Window);
	glfwSwapInterval(0);

	assert(glewInit() == GLEW_OK);

	std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
	std::cout << "OpenGL Vendor  : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GLSL Version   : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	GLuint sphere_num_vertices = 0;
	GLuint sphere_num_indices = 0;

	GLuint programId = loadShaders("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");
	GLuint earthTextureId = loadTexture("textures/earth.jpg");
	GLuint cloudTextureId = loadTexture("textures/cloud.jpg");
	GLuint SquereVao = loadGeometry();
	GLuint SphereVao = loadSphere(sphere_num_vertices, sphere_num_indices);

	std::cout << "Number of vertices in sphere: " << sphere_num_vertices << std::endl;
	std::cout << "Number of indices in sphere: " << sphere_num_indices << std::endl;

	glm::mat4 i = glm::identity<glm::mat4>();
	glm::mat4 model = glm::rotate(i, glm::radians(90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f });

	DirectionalLight Light;
	Light.direction = glm::vec3{ 0.0f, 0.0f, -1.0f };
	Light.intensity = 1.0f;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	double previousTime = glfwGetTime();

	while (!glfwWindowShouldClose(Window)) {
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - previousTime;
		previousTime = currentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programId);

		GLint TimeLoc = glGetUniformLocation(programId, "Time");
		glUniform1f(TimeLoc, currentTime);

		glm::mat4 MVP = Camera.getViewProjection() * model;
		GLint MVPLoc = glGetUniformLocation(programId, "MVP");
		glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		glm::mat4 Normal = glm::inverse(glm::transpose(Camera.getView() * model));
		GLint NormalLoc = glGetUniformLocation(programId, "Normal");
		glUniformMatrix4fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(Normal));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earthTextureId);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, cloudTextureId);

		GLint earthTextureSamplerLoc = glGetUniformLocation(programId, "EarthTextureSampler");
		glUniform1i(earthTextureSamplerLoc, 0);

		GLint cloudTextureSamplerLoc = glGetUniformLocation(programId, "CloudTextureSampler");
		glUniform1i(cloudTextureSamplerLoc, 1);

		GLint lightDirectionLoc = glGetUniformLocation(programId, "LightDirection");
		glUniform3fv(lightDirectionLoc, 1, glm::value_ptr(Camera.getView() * glm::vec4{ Light.direction, 0.0f }));

		GLint lightIntensityLoc = glGetUniformLocation(programId, "LightIntensity");
		glUniform1f(lightIntensityLoc, Light.intensity);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(SphereVao);
		glDrawElements(GL_TRIANGLES, sphere_num_indices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwPollEvents();

		glfwSwapBuffers(Window);

		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
			Camera.moveForward(1.0f * deltaTime);

		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
			Camera.moveForward(-1.0f * deltaTime);

		if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
			Camera.moveRight(-1.0f * deltaTime);

		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
			Camera.moveRight(1.0f * deltaTime);
	}

	glDeleteVertexArrays(1, &SquereVao);

	glfwTerminate();
};