#include "window.hpp"
#include "sprites.hpp"

// Screen dimensions
// TODO: Take this from config file?
constexpr unsigned int screen_width {432};
constexpr unsigned int screen_height {304};
constexpr char window_name [] = "PikaBall MLP Revamped";
constexpr char sprite_sheet_filename [] = "assets/images/sprite_sheet_new.bmp";

constexpr uint32_t sdl_init_flags = SDL_INIT_VIDEO; // | SDL_INIT_AUDIO
constexpr uint64_t sdl_window_flags = SDL_WINDOW_RESIZABLE;

Window::Window() :
      window_(nullptr, SDL_DestroyWindow),
      renderer_(nullptr, SDL_DestroyRenderer)
{
  if (!SDL_Init(sdl_init_flags)) {
    throw std::runtime_error("Failed to init SDL");
  }
  SDL_Window* temp_window;
  SDL_Renderer* temp_renderer;
  if (!SDL_CreateWindowAndRenderer(
        window_name,
        screen_width,
        screen_height,
        sdl_init_flags,
        &temp_window,
        &temp_renderer)
  ) {
    throw std::runtime_error("Failed to create window/renderer");
  }
  window_.reset(temp_window);
  renderer_.reset(temp_renderer);
  // Get pointer to current window surface
  window_surface_ = SDL_GetWindowSurface(window_.get());

  // Set a fixed logical size, so rendering can use fixed pixel coordinates
  if (!SDL_SetRenderLogicalPresentation(
    renderer_.get(),
    screen_width,
    screen_height,
    SDL_LOGICAL_PRESENTATION_STRETCH
  )) {
    SDL_Log("Unable to set a fixed logical scale for the renderer! SDL Error: %s\n", SDL_GetError());
    // Throw??
  }

  // Load sprites and build the static background
  SDL_Log("Loading sprites...");
  load_sprite_sheet();
  generate_background();
  SDL_Log("Done!");
}

Window::~Window() {
  // This might be required to free the window resources before calling SDL_Quit
  // if (window_) {
  //     window_.reset();
  // }

  SDL_Quit();
}

void Window::render() {
  const Uint64 current_time = SDL_GetTicks();
  if (current_time - last_render_time_ < target_time_per_frame_) {
    return;
  }
  // Fill the background white
  SDL_SetRenderDrawColor(renderer_.get(), 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear(renderer_.get());

  // Render image on screen
  if (background_texture_ != nullptr) {
    // Render tests:
    // Render the whole sprite sheet
    // SDL_RenderTexture(renderer_.get(), sprite_sheet_.get(), nullptr, nullptr);
    // Only render the mountain sprite "x":2,"y":200,"w":432,"h":64
    // constexpr SDL_FRect mountain_src(2, 200, 432, 64); // Source is obtained from sprites.hpp
    // constexpr SDL_FRect mountain_dst(0, 188, 432, 64);
    // SDL_RenderTexture(renderer_.get(), sprite_sheet_.get(), &sprite::objects_mountain, &mountain_dst);

    // Render the static background
    SDL_RenderTexture(renderer_.get(), background_texture_.get(), nullptr, nullptr);
    // Waves and clouds
    render_waves();
    render_clouds();
  }

  //Update screen
  SDL_RenderPresent(renderer_.get());
  // SDL_Log("Render time: %d ms - %d fps", current_time -  last_render_time_, 1000 / (current_time -  last_render_time_));
  last_render_time_ = current_time;
}

void Window::load_sprite_sheet() {
  // Load BMP from file, and throw if error
  SDL_Surface* sprites_surface = SDL_LoadBMP(sprite_sheet_filename);
  if (sprites_surface == nullptr) {
    SDL_Log( "Unable to load image %s! SDL Error: %s\n", sprite_sheet_filename, SDL_GetError());
    throw std::runtime_error("Failed to load sprite sheet!");
  }
  // Generate the texture and save it
  sprite_sheet_.reset(SDL_CreateTextureFromSurface(renderer_.get(), sprites_surface));
  SDL_Log("Sprite sheet size: %dx%d", sprite_sheet_.get()->w, sprite_sheet_.get()->h);
  // Release the temporary surface object
  SDL_DestroySurface(sprites_surface);
}

void Window::generate_background() {
  // Create a target texture to render all the background elements to
  background_texture_.reset(SDL_CreateTexture(
    renderer_.get(),
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_TARGET,
    screen_width,
    screen_height
  ));
  // Focus the renderer on the target texture
  SDL_SetRenderTarget(renderer_.get(), background_texture_.get());

  // Fill the background white
  SDL_SetRenderDrawColor(renderer_.get(), 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear(renderer_.get());

  // Build the sky
  SDL_FRect f_dst;
  SDL_Rect dst = {
    // Preset width and height of sky texture
    .w = 16,
    .h = 16,
  };
  for (int i = 0; i < screen_width / 16; i++) {
    for (int j = 0; j < 12; j++) {
      // Set position and render texture
      dst.x = i * 16;
      dst.y = j * 16;
      SDL_RectToFRect(&dst, &f_dst);
      SDL_RenderTexture(
        renderer_.get(), sprite_sheet_.get(), &sprite::objects_sky_blue, &f_dst);
    }
  }
  // Render the mountain sprite
  dst.x = 0;
  dst.y = 188;
  dst.w = 432;
  dst.h = 64;
  SDL_RectToFRect(&dst, &f_dst);
  SDL_RenderTexture(
    renderer_.get(), sprite_sheet_.get(), &sprite::objects_mountain, &f_dst);

  // Render the red ground
  dst.y = 248;
  dst.w = 16;
  dst.h = 16;
  for (int i = 0; i < screen_width / 16; i++) {
    dst.x = i * 16;
    SDL_RectToFRect(&dst, &f_dst);
    SDL_RenderTexture(
      renderer_.get(), sprite_sheet_.get(), &sprite::objects_ground_red, &f_dst);
  }

  // Render the ground line (the field delimiters)
  dst.x = 0;
  dst.y = 264;
  SDL_RectToFRect(&dst, &f_dst);
  SDL_RenderTexture(
    renderer_.get(), sprite_sheet_.get(), &sprite::objects_ground_line_leftmost, &f_dst);
  for (int i = 1; i < screen_width / 16 - 1; i++) {
    dst.x = i * 16;
    SDL_RectToFRect(&dst, &f_dst);
    SDL_RenderTexture(
      renderer_.get(), sprite_sheet_.get(), &sprite::objects_ground_line, &f_dst);
  }
  dst.x = screen_width - 16;
  dst.y = 264;
  SDL_RectToFRect(&dst, &f_dst);
  SDL_RenderTexture(
    renderer_.get(), sprite_sheet_.get(), &sprite::objects_ground_line_rightmost, &f_dst);

  // Render the yellow ground
  for (int i = 0; i < screen_width / 16; i++) {
    for (int j = 0; j < 2; j++) {
      dst.x = i * 16;
      dst.y = 280 + j * 16;
      SDL_RectToFRect(&dst, &f_dst);
      SDL_RenderTexture(
        renderer_.get(), sprite_sheet_.get(), &sprite::objects_ground_yellow, &f_dst);
    }
  }

  // Render the net (pillar and top)
  dst.x = 213;
  dst.y = 176;
  dst.w = 8;
  dst.h = 8;
  SDL_RectToFRect(&dst, &f_dst);
  SDL_RenderTexture(
    renderer_.get(), sprite_sheet_.get(), &sprite::objects_net_pillar_top, &f_dst);
  for (int j = 0; j < 12; j++) {
    dst.y = 184 + j * 8;
    SDL_RectToFRect(&dst, &f_dst);
    SDL_RenderTexture(
      renderer_.get(), sprite_sheet_.get(), &sprite::objects_net_pillar, &f_dst);
  }

  // Set the render target back to the main window
  SDL_SetRenderTarget(renderer_.get(), nullptr);
}

void Window::render_waves() {
  wave_.update();
  SDL_FRect f_dst;
  SDL_Rect dst = {
    .x = 0,
    .w = 16,
    .h = 32,
  };
  for (const auto& w : wave_.get_coords()) {
    dst.y = w;
    SDL_RectToFRect(&dst, &f_dst);
    SDL_RenderTexture(
      renderer_.get(), sprite_sheet_.get(), &sprite::objects_wave, &f_dst);
    dst.x += dst.w;
  }
}

void Window::render_clouds() {
  clouds_.update();
  SDL_FRect f_dst;
  for (const auto& cloud : clouds_.get_clouds()) {
    SDL_RectToFRect(&cloud, &f_dst);
    SDL_RenderTexture(
      renderer_.get(), sprite_sheet_.get(), &sprite::objects_cloud, &f_dst);
  }
}
