#pragma once


struct Image
{
	// color channels: 3 -> rbg, 4 -> rgba, etc. you get it
	int32 channels{ 3 };
	int32 image_id{ -1 };

	int32 width{ -1 };
	int32 height{ -1 };
};

struct Sampler
{
    enum class Filter : int32
    {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear
    };

    enum class Wrap : int32
    {
        Repeat,
        ClampToEdge,
        MirroredRepeat
    };

	// default value from gltf2.0 standard lol
    Filter mag_filter{ Filter::Nearest };
    Filter min_filter{ Filter::Nearest };

    Wrap wrap_s{ Wrap::ClampToEdge };
    Wrap wrap_t{ Wrap::ClampToEdge };
};

struct Texture
{
    std::shared_ptr<Image> image;
    std::shared_ptr<Sampler> sampler;
};
