#ifndef GH_BF4_PLAYER_HPP
#define GH_BF4_PLAYER_HPP

#include <array>
#include <cstdint>
#include <string>

#include <glm/vec3.hpp>

namespace gh
{
struct player
{
  bool                       local         ;
  std::string                name          ;
  float                      health        ;
  std::uint32_t              team          ;
  glm::vec3                  position      ;
  std::array<glm::vec3, 200> bone_positions;
};
}

#endif