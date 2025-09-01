#include <glad/glad.h> // Make sure you have glad installed for function loading

#include <GLFW/glfw3.h>
#include <iostream>

// Error callback for GLFW
void errorCallback(int error, const char *description) {
  std::cerr << "GLFW Error [" << error << "]: " << description << "\n";
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main() {
  std::cout << "Starting OpenGL Triangle.." << std::endl;

  // Set GLFW error callback
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }

  // Request OpenGL 3.3 Core Profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create window
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "OpenGL Triangle", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }

  // Make context current
  glfwMakeContextCurrent(window);

  // Load OpenGL functions using GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
  }

  // Build and compile shaders
  const char *vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        
        out vec3 ourColor;
        void main() {
            gl_Position = vec4(aPos, 1.0);
            ourColor = aColor;
        }
    )";

  const char *fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 ourColor;
        void main() {
            FragColor = vec4(ourColor, 1.0);
        }
    )";

  // Vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cerr << "ERROR::VERTEX_SHADER_COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // Fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    std::cerr << "ERROR::FRAGMENT_SHADER_COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // Shader program
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cerr << "ERROR::SHADER_PROGRAM_LINKING_FAILED\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Set up vertex data and buffers
  float vertices[] = {
      // positions         // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
  };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // Bind VAO
  glBindVertexArray(VAO);

  // Bind VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Vertex attribute pointer
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    // Clear screen
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
