#ifndef GH_BF4_WALLHACK_HPP
#define GH_BF4_WALLHACK_HPP

#include <array>
#include <memory>

#include <gh/core/system.hpp>
#include <gh/memory/process.hpp>
#include <glm/glm.hpp>

#include <gh_bf4/player.hpp>

namespace gh
{
class wallhack : public system
{
public:
  const std::array<player, 64>& players               () const;
  const glm::mat4&              view_projection_matrix() const;

protected:
  void prepare              (                                    scene* scene) override;
  void update               (const frame_timer::duration& delta, scene* scene) override;

  std::unique_ptr<process> process_               ;
  std::uint64_t            game_context_          ;
  std::uint64_t            player_manager_        ;
  std::uint64_t            game_renderer_         ;
  std::uint64_t            render_view_           ;
  std::uint64_t            local_player_offset_   ;
  std::uint64_t            players_offset_        ;
  
  std::array<player, 64>   players_               ;
  glm::mat4                view_projection_matrix_;
};
}

#endif