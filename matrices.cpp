#include <iostream>
#include <iomanip>
#include <glm\glm.hpp>
#include <glm/gtx/string_cast.hpp>

void printMatrix(glm::mat4& M) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << std::setw(10) << std::setprecision(4) << std::fixed << M[j][i];
		}
		std::cout << std::endl;
	}
}

void translationMatrix() {
	glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), { 10, 10, 10 });

	glm::vec4 position{ 10, 10, 10, 1 };
	glm::vec4 direction{ 10, 10, 10, 0 };

	position = translation * position;

	std::cout << glm::to_string(position) << std::endl;

	printMatrix(translation);
}

void scaleMatrix() {
	glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), { 2, 2, 2 });

	printMatrix(scale);
}

void rotationMatrix() {
	glm::mat4 rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(90.0f), { 0, 0, 1 });

	glm::vec4 a{ 200, 50, 0, 1 };
	a = rotation * a;
	std::cout << glm::to_string(a) << std::endl;
}

void composedMatrices() {
	glm::mat4 I = glm::identity<glm::mat4>();

	glm::vec3 T{ 1, 10, 7 };
	glm::mat4 translation = glm::translate(I, T);

	glm::vec3 S{ 2, 2, 7 };
	glm::mat4 scale = glm::scale(I, S);

	constexpr float A = glm::radians(45.0f);
	glm::vec3 axis{ 1, 1, 1 };
	glm::mat4 rotation = glm::rotate(I, A, axis);

	printMatrix(translation);
	std::cout << std::endl;
	printMatrix(scale);
	std::cout << std::endl;
	printMatrix(rotation);
	std::cout << std::endl;

	glm::mat4 modelMatrix = translation * rotation * scale;
	printMatrix(modelMatrix);

}

void modelViewProject() {
	glm::mat4 model = glm::identity<glm::mat4>();

	glm::vec3 eye{ 0, 0, 10 }, center{ 0, 0, 0 }, up{ 0, 1, 0 };
	glm::mat4 view = glm::lookAt(eye, center, up);

	constexpr float fov = glm::radians(45.0f);
	const float aspectRatio = 16.0f / 9.0f;
	const float near = 0.01f;
	const float far = 100000.0f;
	glm::mat4 projection = glm::perspective(fov, aspectRatio, near, far);

	glm::mat4 modelViewProjection = projection * view * model;

	printMatrix(modelViewProjection);

	glm::vec4 position{ 0, 0, 0, 1 };
	glm::vec4 direction{ 0, 0, 0, 0 };

	position = modelViewProjection * position;
	direction = modelViewProjection * direction;

	std::cout << glm::to_string(position) << std::endl << glm::to_string(direction);
}

void main() {
	modelViewProject();
}
