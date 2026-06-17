#pragma once


// Rendering mock up:
// 
// Base Pass
// Post Process Pass
//

struct Framebuffer;

// Rendering subsystem base class
class Subsystem
{
public:
	Subsystem() = default;
	virtual ~Subsystem() {};

	// override these to render
	// implement rendering in device specific source file
	virtual void base_pass() {};
	virtual void post_process_pass() {};

	virtual void tick(float _delta_time) {};

	std::shared_ptr<Framebuffer> framebuffer;
protected:
	uint32 priority{ 0 };

	friend class Renderer;
};