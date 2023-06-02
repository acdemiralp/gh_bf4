#include <gh_bf4/aimbot.hpp>

#include <gh/core/engine.hpp>
#include <gh/display/display_system.hpp>
#include <gh/graphics/imgui_context.hpp>
#include <gh/input/input_system.hpp>
#include <gh/math/bezier.hpp>

#include <gh_bf4/project.hpp>
#include <gh_bf4/wallhack.hpp>

namespace gh
{
void aimbot::prepare(                                    scene* scene)
{
  auto input_system = engine_->system<gh::input_system>();
  input_system->on_key_press  .connect([&] (const key_code key)
  {
    if (key != key_code::c)
      return;

    const auto wallhack               = engine_->system<gh::wallhack>();
    const auto view_projection_matrix = wallhack->view_projection_matrix();

    const auto window_size            = engine_->system<display_system>()->windows()[0]->size();
    const auto viewport_size          = glm::vec2(window_size[0], window_size[1]);
    const auto center                 = viewport_size * 0.5F;

    auto       target_screen_position = glm::vec2();

    auto& players      = wallhack->players();
    auto& local_player = *std::find_if(players.begin(), players.end(), [ ] (const player& player) { return player.local; });
    for (auto& candidate : players)
    {
      if (candidate.local || candidate.health == 0 || candidate.team == local_player.team)
        continue;

      auto candidate_screen_position = project(candidate.bone_positions[142], view_projection_matrix, viewport_size);
      if (glm::length(candidate_screen_position - center) > 75.0f)
        continue;

      if (!target_ || glm::length(target_screen_position - center) > glm::length(candidate_screen_position - center))
      {
        target_                = &candidate;
        target_screen_position = candidate_screen_position;
      }
    }

    if (target_)
      bezier_points_ = { center, center + glm::vec2(rng_.generate(), rng_.generate()), target_screen_position + glm::vec2(rng_.generate(), rng_.generate()), target_screen_position };
  });
  input_system->on_key_release.connect([&] (const key_code key)
  {
    if (key != key_code::c)
      return;

    target_           = nullptr;
    current_duration_ = frame_timer::duration(0);
  });

  ImGui::SetCurrentContext(imgui_context());
}
void aimbot::update (const frame_timer::duration& delta, scene* scene)
{
  if (!target_ || target_->health == 0) return;
  
  const auto wallhack               = engine_->system<gh::wallhack>();
  const auto view_projection_matrix = wallhack->view_projection_matrix();

  const auto window_size            = engine_->system<display_system>()->windows()[0]->size();
  const auto viewport_size          = glm::vec2(window_size[0], window_size[1]);
  const auto center                 = viewport_size * 0.5F;

  auto       coordinates            = project(target_->bone_positions[142], view_projection_matrix, viewport_size);

  current_duration_ += delta;
  if (current_duration_ < bezier_duration_)
  {
    bezier_points_.back() = coordinates;
    coordinates = gh::bezier(bezier_points_, current_duration_.count() / bezier_duration_.count());
  }
  
  engine_->system<input_system>()->move_mouse({std::int32_t(coordinates[0]), std::int32_t(coordinates[1])}, true);

  
  imgui_begin_scene();
  auto draw_list = ImGui::GetWindowDrawList();
  if (target_)
  {
    for (auto i = current_duration_.count(); i <= bezier_duration_.count(); ++i)
    {
      auto point = gh::bezier(bezier_points_, i / bezier_duration_.count());
      draw_list->AddCircleFilled({point[0], point[1]}, 2, ImGui::GetColorU32({1.0f, 0.8f, 0.0f, 1.0f}));
    }

#ifdef _DEBUG
    for (auto& point : bezier_points_)
      draw_list->AddCircleFilled({point[0], point[1]}, 4, ImGui::GetColorU32({1.0f, 0.8f, 0.0f, 1.0f}));
#endif
  }
  imgui_end_scene  ();
}
}