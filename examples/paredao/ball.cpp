#include "ball.hpp"

#include <glm/gtx/fast_trigonometry.hpp>

void Ball::create(GLuint program) {
  destroy();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_polygonSides = 20;

  m_translation = {0, -0.9};

  auto &re{m_randomEngine}; // Shortcut
  std::uniform_real_distribution randomIntensity(0.5f, 1.0f);
  m_color = glm::vec4(randomIntensity(re));

  m_color.a = 1.0f;

  // Get a random direction
  glm::vec2 direction{m_randomDist(re), m_randomDist(re)};
  if (direction.y < 0) {
    direction.y = m_randomDist(re);
  }
  m_velocity = glm::normalize(direction) / 3.0f;

  // Create geometry data
  std::vector<glm::vec2> positions{{0, 0}};
  auto const step{M_PI * 2 / m_polygonSides};
  std::uniform_real_distribution randomRadius(0.8f, 1.0f);
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(0.3 * std::cos(angle), 0.3 * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Ball::paint() {
  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glUniform1f(m_scaleLoc, m_scale);

  abcg::glUniform2f(m_translationLoc, m_translation.x, m_translation.y);

  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, m_polygonSides + 2);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ball::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Ball::update(const Bar &bar, float deltaTime) {
  m_translation -= bar.m_velocity * deltaTime;
  m_translation += m_velocity * deltaTime;

  // Colisões nas paredes
  if (m_translation.x < -0.97f)
    m_velocity.x = -m_velocity.x;
  if (m_translation.x > +0.97f)
    m_velocity.x = -m_velocity.x;
  if (m_translation.y > +0.97f)
    m_velocity.y = -m_velocity.y;
}