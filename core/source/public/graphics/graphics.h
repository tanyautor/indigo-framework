#pragma once


// TODO: this cannot be resized yet, so please for the love of all that is holy!!!!
struct Framebuffer
{
	Framebuffer(const char* rdg_label = nullptr);
	~Framebuffer();

	// read_write correspond to either GL_TEXTURE_2D, or GL_RENDERBUFFER
	bool init_color_buffer(uint32 _access);
	bool init_depth_stencil(uint32 _access);

	bool check_complete();

	uint32 fbo{ 0 };

	/// attached textures
	// please always attach a color texture...
	// depth and stencil can be packed into one, or attached to the color buffer, so using optional here

	std::optional<uint32> color_buffer{ std::nullopt };
	std::optional<uint32> depth_stencil{ std::nullopt };

	uint32 color_buffer_access{ 0 };
	uint32 depth_stencil_access{ 0 };

	uint32 width{ 0 };
	uint32 height{ 0 };

private:
	std::string rdg_label{};
};

class Window
{
public:
	Window(uint32 _width = 800u, uint32 _height = 600u, const char* _title = "you're boring", bool _fullscreen = false);
	~Window();

	const glm::ivec2& get_window_size() const;
	bool is_running() const;

	void begin_frame();
	void end_frame();

private:
	struct Impl;
	std::unique_ptr<Impl> pImpl;
};