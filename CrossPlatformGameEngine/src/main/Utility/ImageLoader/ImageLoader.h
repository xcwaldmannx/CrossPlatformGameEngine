#pragma once

#include <vector>

struct RawImage {
	char* mFilepath;
	int mWidth;
	int mHeight;
	int mChannels;
	std::vector<unsigned char> mPixels;
};

class ImageLoader {
public:
	static void loadImage(const char* filepath, RawImage* image);
};