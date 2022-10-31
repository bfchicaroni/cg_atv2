#ifndef BALLS_HPP_
#define BALLS_HPP_

#include <list>
#include <random>

#include "abcgOpenGL.hpp"

#include "bar.hpp"
#include "gamedata.hpp"

class Balls {
public:
  void create(GLuint program, int quantity);
  void paint();
  void destroy();
  void update(const Bar &bar, float deltaTime);

  struct Ball {
    GLuint m_VAO{};
    GLuint m_VBO{};

    glm::vec4 m_color{1};
    bool m_hit{};
    int m_polygonSides{};
    float m_scale{};
    glm::vec2 m_translation{};
    glm::vec2 m_velocity{};
  };

  std::list<Ball> m_ball;

  Ball makeBall(glm::vec2 translation = {}, float scale = 0.25f);

private:
  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
};

#endif