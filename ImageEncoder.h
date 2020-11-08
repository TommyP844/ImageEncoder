#pragma once

class ImageEncoder
{
public:
	ImageEncoder();
	~ImageEncoder();

	int encode(const char* dataFile, const char* imageFile, const char* outputFile);
	int decode(const char* imageFile, const char* outputFile);

	int help();

private:
	enum class FileExtension
	{
		FT_TGA = 0,
		FT_PNG = 1,
		FT_BMP = 2
	};

	unsigned char SetBit(unsigned char byte, int pos, int value);
	unsigned char ReadBit(unsigned char byte, int pos);

	unsigned char* LoadBinaryFile(const char* filename, int& size);

	unsigned char* LoadImage(const char* filename, int& width, int& height, int& comp);

	void FreeImage(void* image);

	int SaveImage(const char* filename, unsigned char* data,
		int width, int height, int comp, FileExtension ft);

};

