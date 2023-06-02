#include <gh/api.hpp>
#include <gh/graphics/backend/opengl/render_tasks.hpp>

#include <gh_bf4/aimbot.hpp>
#include <gh_bf4/wallhack.hpp>

int main(int argc, char** argv)
{
  auto engine = gh::make_default_engine();
  auto window = engine->system<gh::display_system>()->create_opengl_window("GH BF4", di::opengl_context_settings(), di::window_flags::always_on_top | di::window_flags::skip_taskbar);
  window->set_mode       (di::window_mode::fullscreen_windowed);
  window->set_bordered   (false);
  window->set_layered    (true );
  window->set_transparent(true );
  //window->set_transparency_color({0, 0, 0});
  window->set_swap_mode  (di::opengl_swap_mode::vertical_sync);

  gh::opengl::make_default_framegraph(engine.get(), window);
  gh::console::set_visible(false);

  engine->add_system<gh::aimbot   >();
  engine->add_system<gh::wallhack >();

  engine->run();
  return 0;
}