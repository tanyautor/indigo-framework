#pragma once


// Rendering mock up:
// 
// Base Pass
// Post Process Pass
//

struct Framebuffer;

class SubsystemBase
{
public:
	SubsystemBase() = default;
	virtual ~SubsystemBase() {};

	// override these to render
	// implement rendering in device specific source file
	virtual void base_pass() {};
	virtual void post_process_pass() {};

	virtual void tick(float _delta_time) {};

	Framebuffer* framebuffer{ nullptr };
protected:
	uint32 priority{ 0 };

	friend class Renderer;
};