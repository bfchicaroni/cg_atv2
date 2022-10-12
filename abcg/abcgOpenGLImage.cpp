/**
 * @file abcgOpenGLImage.cpp
 * @brief Definition of OpenGL texture loading helper functions.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2022 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgOpenGLImage.hpp"
#include "abcgImage.hpp"

#include <cppitertools/itertools.hpp>
#include <fmt/core.h>
#include <fstream>
#include <gsl/gsl>
#include <vector>

#include "abcgException.hpp"

GLuint abcg::loadOpenGLTexture(OpenGLTextureCreateInfo const &createInfo) {
  GLuint textureID{};

  if (SDL_Surface *const surface{IMG_Load(createInfo.path.data())}) {
    // Enforce RGB/RGBA
    GLenum internalFormat{};
    GLenum format{};
    SDL_Surface *formattedSurface{};
    if (surface->format->BytesPerPixel == 3) {
      formattedSurface =
          SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB24, 0);
      internalFormat = createInfo.sRGBToLinear ? GL_SRGB8 : GL_RGB;
      format = GL_RGB;
    } else {
      formattedSurface =
          SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
      internalFormat = createInfo.sRGBToLinear ? GL_SRGB8_ALPHA8 : GL_RGBA;
      format = GL_RGBA;
    }
    SDL_FreeSurface(surface);

    // Flip upside down
    if (createInfo.flipUpsideDown) {
      flipVertically(formattedSurface);
    }

    // Generate the texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, gsl::narrow<GLint>(internalFormat),
                 formattedSurface->w, formattedSurface->h, 0, format,
                 GL_UNSIGNED_BYTE, formattedSurface->pixels);

    SDL_FreeSurface(formattedSurface);

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate the mipmap levels
    if (createInfo.generateMipmaps) {
      glGenerateMipmap(GL_TEXTURE_2D);

      // Override minifying filtering
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
    }

    // Set texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  } else {
    throw abcg::RuntimeError(
        fmt::format("Failed to load texture file {}", createInfo.path));
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  return textureID;
}

GLuint abcg::loadOpenGLCubemap(OpenGLCubemapCreateInfo const &createInfo) {
  GLuint textureID{};
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  for (auto &&[index, path] : iter::enumerate(createInfo.paths)) {
    // Load the bitmap
    if (SDL_Surface *const surface{IMG_Load(path.data())}) {
      // Enforce RGB
      SDL_Surface *const formattedSurface{
          SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB24, 0)};
      SDL_FreeSurface(surface);

      auto target{GL_TEXTURE_CUBE_MAP_POSITIVE_X + gsl::narrow<GLenum>(index)};

      // LHS to RHS
      if (createInfo.rightHandedSystem) {
        if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y) {
          // Flip upside down
          flipVertically(formattedSurface);
        } else {
          flipHorizontally(formattedSurface);
        }

        // Swap -z and +z
        if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z)
          target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        else if (target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
          target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
      }

      // Create texture
      glTexImage2D(target, 0, GL_RGB, formattedSurface->w, formattedSurface->h,
                   0, GL_RGB, GL_UNSIGNED_BYTE, formattedSurface->pixels);

      SDL_FreeSurface(formattedSurface);
    } else {
      throw abcg::RuntimeError(
          fmt::format("Failed to load texture file {}", path));
    }
  }

  // Set texture wrapping
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Set texture filtering
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Generate the mipmap levels
  if (createInfo.generateMipmaps) {
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Override minifying filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
  }

  return textureID;
}