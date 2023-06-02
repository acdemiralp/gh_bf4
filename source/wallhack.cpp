#include <gh_bf4/wallhack.hpp>

#include <algorithm>

#include <gh/core/engine.hpp>
#include <gh/display/display_system.hpp>
#include <gh/graphics/imgui_context.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <gh_bf4/aimbot.hpp>
#include <gh_bf4/offsets.hpp>
#include <gh_bf4/project.hpp>

namespace gh
{
const std::array<player, 64>& wallhack::players               () const
{
  return players_;
}
const glm::mat4&              wallhack::view_projection_matrix() const
{
  return view_projection_matrix_;
}

void wallhack::prepare(                                    scene* scene)
{
  process_             = std::make_unique<process>("Battlefield 4", true);

  game_context_        = process_->read<std::uint64_t>(offsets::client_game_context);
  player_manager_      = process_->read<std::uint64_t>(game_context_  + 0x60);
  local_player_offset_ = process_->read<std::uint64_t>(player_manager_ + 0x540);
  players_offset_      = process_->read<std::uint64_t>(player_manager_ + 0x548);
  
  game_renderer_       = process_->read<std::uint64_t>(offsets::client_game_renderer);
  render_view_         = process_->read<std::uint64_t>(game_renderer_ + 0x60);

  ImGui::SetCurrentContext(imgui_context());
}
void wallhack::update (const frame_timer::duration& delta, scene* scene)
{
  const std::vector<std::uint32_t> bone_indices
  {
    104, // head
    142, // neck
    9  , // left arm
    11 , // left elbow
    15 , // left hand
    109, // right arm
    111, // right elbow
    115, // right hand
    5  , // groin
    188, // left knee
    184, // left foot
    197, // right knee
    198  // right foot
  };
  const std::vector<std::array<std::uint32_t, 2>> joints
  {
    {104, 142}, 
    {142, 9  }, 
    {9  , 11 }, 
    {11 , 15 },
    {142, 109}, 
    {109, 111}, 
    {111, 115}, 
    {142, 5  },
    {5  , 188},
    {188, 184},
    {5  , 197}, 
    {197, 198}
  };

  for (std::size_t i = 0; i < 64; ++i)
  {
    const auto player          = process_->read<std::uint64_t>(players_offset_ + i * 0x8  );
    const auto soldier         = process_->read<std::uint64_t>(player          + 0x14D0   );
    const auto health_offset   = process_->read<std::uint64_t>(soldier         + 0x140    );
    const auto position_offset = process_->read<std::uint64_t>(soldier         + 0x490    );
    const auto ragdoll_offset  = process_->read<std::uint64_t>(soldier         + 0x580    );
    const auto bone_offset     = process_->read<std::uint64_t>(ragdoll_offset  + 0xB0     );
                               
    players_[i].local          = player == local_player_offset_;
    players_[i].name           = process_->sread<>            (player          + 0x40, 64);
    players_[i].health         = process_->read<float>        (health_offset   + 0x20    );
    players_[i].team           = process_->read<std::uint64_t>(player          + 0x13CC  );
    players_[i].position       = process_->read<glm::vec3>    (position_offset + 0x30    );
    for (auto j = 0; j < bone_indices.size(); ++j)
      players_[i].bone_positions[bone_indices[j]] = process_->read<glm::vec3>(bone_offset + bone_indices[j] * 0x20);
  }
  view_projection_matrix_ = process_->read<glm::mat4>(render_view_ + 0x420);

#ifdef _DEBUG
  ImGui::Begin("Debug");
  for (auto i = 0; i < 64; ++i)
    if (players_[i].health != 0)
      ImGui::Text(
        "name: %s, health: %f, team: %i, position: %f %f %f, local: %s", 
        players_[i].name  .c_str(), 
        players_[i].health        , 
        players_[i].team          , 
        players_[i].position[0]   , 
        players_[i].position[1]   , 
        players_[i].position[2]   , 
        players_[i].local ? "true" : "false");
  ImGui::End();
#endif
  
  const auto aimbot        = engine_->system<gh::aimbot>();
  const auto window_size   = engine_->system<display_system>()->windows()[0]->size();
  const auto viewport_size = glm::vec2(window_size[0], window_size[1]);
  
  imgui_begin_scene();
  auto draw_list = ImGui::GetWindowDrawList();
  auto& local = *std::find_if(players_.begin(), players_.end(), [ ] (const player& player) { return player.local; });
  for (auto& player : players_)
  {
    if (player.local || player.health == 0 || glm::dot(glm::vec3(glm::column(view_projection_matrix_, 2)), glm::normalize(player.position - local.position)) < 0.0f)
      continue;

    auto color = player.team == local.team ? ImGui::GetColorU32({0.0f, 0.5f, 1.0f, 1.0f}) : ImGui::GetColorU32({1.0f, 0.5f, 0.0f, 1.0f});
    if (&player == aimbot->target()) color = ImGui::GetColorU32({1.0f, 0.8f, 0.0f, 1.0f});

    for (auto& joint : joints)
    {
      const auto a = project(player.bone_positions[joint[0]], view_projection_matrix_, viewport_size);
      const auto b = project(player.bone_positions[joint[1]], view_projection_matrix_, viewport_size);   
      draw_list->AddLine({a[0], a[1]}, {b[0], b[1]}, color, 4);
    }
  }
  imgui_end_scene  ();
}
}
