#include "ball.hpp"

#include <glm/gtx/fast_trigonometry.hpp>

void Balls::create(GLuint program, int quantity) {
  destroy();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create ball
  m_ball.clear();
  m_ball.resize(quantity);

  for (auto &ball : m_ball) {
    ball = makeBall();

    // Make sure the ball won't collide with the bar
    do {
      ball.m_translation = {m_randomDist(m_randomEngine),
                            m_randomDist(m_randomEngine)};
    } while (glm::length(ball.m_translation) < 0.5f);
  }
}

void Balls::paint() {
  abcg::glUseProgram(m_program);

  for (auto const &ball : m_ball) {
    abcg::glBindVertexArray(ball.m_VAO);

    abcg::glUniform4fv(m_colorLoc, 1, &ball.m_color.r);
    abcg::glUniform1f(m_scaleLoc, ball.m_scale);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        abcg::glUniform2f(m_translationLoc, ball.m_translation.x + j,
                          ball.m_translation.y + i);

        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, ball.m_polygonSides + 2);
      }
    }

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Balls::destroy() {
  for (auto &ball : m_ball) {
    abcg::glDeleteBuffers(1, &ball.m_VBO);
    abcg::glDeleteVertexArrays(1, &ball.m_VAO);
  }
}

void Balls::update(const Bar &bar, float deltaTime) {
  for (auto &ball : m_ball) {
    ball.m_translation -= bar.m_velocity * deltaTime;
    ball.m_translation += ball.m_velocity * deltaTime;

    // Wrap-around
    if (ball.m_translation.x < -1.0f)
      ball.m_translation.x += 2.0f;
    if (ball.m_translation.x > +1.0f)
      ball.m_translation.x -= 2.0f;
    if (ball.m_translation.y < -1.0f)
      ball.m_translation.y += 2.0f;
    if (ball.m_translation.y > +1.0f)
      ball.m_translation.y -= 2.0f;
  }
}

Balls::Ball Balls::makeBall(glm::vec2 translation, float scale) {
  Ball ball;

  auto &re{m_randomEngine}; // Shortcut

  // Randomly pick the number of sides
  ball.m_polygonSides = 20;

  // Get a random color (actually, a grayscale)
  std::uniform_real_distribution randomIntensity(0.5f, 1.0f);
  ball.m_color = glm::vec4(randomIntensity(re));

  ball.m_color.a = 1.0f;
  ball.m_scale = scale;
  ball.m_translation = translation;

  // Get a random direction
  glm::vec2 const direction{m_randomDist(re), m_randomDist(re)};
  ball.m_velocity = glm::normalize(direction) / 7.0f;

  // Create geometry data
  std::vector<glm::vec2> positions{{0, 0}};
  auto const step{M_PI * 2 / ball.m_polygonSides};
  std::uniform_real_distribution randomRadius(0.8f, 1.0f);
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    auto const radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &ball.m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, ball.m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &ball.m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(ball.m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, ball.m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return ball;
}