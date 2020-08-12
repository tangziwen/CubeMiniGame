#pragma once


namespace tzw {
enum class ImageFormat
{
	R8,
	R8G8,
	R8G8B8,
	R8G8B8A8,
	R16G16B16A16,
	D24_S8,
};
size_t ImageFormatGetSize(ImageFormat format);
} // namespace tzw

