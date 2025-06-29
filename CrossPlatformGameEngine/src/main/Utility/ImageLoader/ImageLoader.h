#pragma once

struct RawImage {
	char* mFilepath;
	int mWidth;
	int mHeight;
	int mChannels;
	unsigned char* mPixels = nullptr;
};

class ImageLoader {
public:
	static void loadImage(const char* filepath, RawImage* image);
	static void unloadImage(RawImage& image);
};