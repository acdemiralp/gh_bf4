#ifndef GH_BF4_OFFSETS_HPP
#define GH_BF4_OFFSETS_HPP

#include <cstdint>

namespace gh
{
namespace offsets
{
constexpr std::intptr_t client_game_context          = 0x142670d80;
constexpr std::intptr_t client_dx_renderer           = 0x142738080;
constexpr std::intptr_t client_game_renderer         = 0x142672378;
constexpr std::intptr_t client_angles                = 0x1423b2ec0;
constexpr std::intptr_t client_world_render_settings = 0x1426724a0;
}
}

#endif