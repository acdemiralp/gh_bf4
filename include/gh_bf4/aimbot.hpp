#ifndef GH_BF4_AIMBOT_HPP
#define GH_BF4_AIMBOT_HPP

#include <random>
#include <vector>

#include <acd/random_number_generator.hpp>
#include <gh/core/system.hpp>
#include <glm/glm.hpp>

#include <gh_bf4/player.hpp>

namespace gh
{
class aimbot : public system
{
public:
  player const* target() const
  {
    return target_;
  }
  
protected:
  void prepare(                                    scene* scene) override;
  void update (const frame_timer::duration& delta, scene* scene) override;

  player const*          target_           = nullptr;
  std::vector<glm::vec2> bezier_points_    = {};
  frame_timer::duration  bezier_duration_  = frame_timer::duration(500);
  frame_timer::duration  current_duration_ = frame_timer::duration(0  );

  acd::random_number_generator<std::uniform_real_distribution<float>> rng_ = acd::random_number_generator<std::uniform_real_distribution<float>>(-1.0f, 1.0f);
};
}

#endif