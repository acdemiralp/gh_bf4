#ifndef GH_BF4_PROJECT_HPP
#define GH_BF4_PROJECT_HPP

#include <glm/glm.hpp>

namespace gh
{
inline glm::vec2 project(const glm::vec3& position, const glm::mat4& view_projection_matrix, const glm::vec2& viewport_size)
{
	glm::vec4 temp(position, 1.0);
	temp    = view_projection_matrix * temp;
  temp[0] = 0.5f * viewport_size[0] * (1.0f + temp[0] / temp[3]);
  temp[1] = 0.5f * viewport_size[1] * (1.0f - temp[1] / temp[3]);
	return {temp[0], temp[1]};
}
}

#endif