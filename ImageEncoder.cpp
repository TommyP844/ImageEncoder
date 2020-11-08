
#include "ImageEncoder.h"


#include <iostream>
#include <fstream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_writer.h"


ImageEncoder::ImageEncoder()
{
}

ImageEncoder::~ImageEncoder()
{
}

int ImageEncoder::encode(const char* dataFile, const char* imageFile, const char* outputFile)
{
	FileExtension ft;

	if (std::string(outputFile).find(".bmp") != std::string::npos)
		ft = FileExtension::FT_BMP;
	else if (std::string(outputFile).find(".png") != std::string::npos)
		ft = FileExtension::FT_PNG;
	else if (std::string(outputFile).find(".tga") != std::string::npos)
		ft = FileExtension::FT_TGA;
	else
	{
		std::cout << "Invalid file type" << std::endl;
		return -1;
	}

	unsigned char* datafile = 0;
	int imageWidth = 0, imageHeight = 0, imageComp = 0;
	unsigned char* image = LoadImage(imageFile, imageWidth, imageHeight, imageComp);
	
	if (image == nullptr)
		return -1;

	int encodingDataLength = 0;
	unsigned char* encodingData = LoadBinaryFile(dataFile, encodingDataLength);

	if (encodingData == nullptr)
		return -1;

	// can we complete this task?
	int storageAvailable = imageWidth * imageComp * imageHeight / 4;
	if (encodingDataLength + 16 > storageAvailable)
	{
		std::cout << "Storage Available: " << storageAvailable << std::endl;
		std::cout << "Image file to small." << std::endl;
		return 2;
	}

	unsigned char split[4];
	for(int i = 0; i < 4; i++)
		split[i] = (unsigned char)((encodingDataLength >> (8 * i)) & 0xff);

	for (int i = 0; i < 4; i++)
	{
		unsigned char byte = split[i];
		image[i * 4 + 0] = SetBit(image[i * 4 + 0], 1, ReadBit(byte, 7));
		image[i * 4 + 0] = SetBit(image[i * 4 + 0], 0, ReadBit(byte, 6));
		image[i * 4 + 1] = SetBit(image[i * 4 + 1], 1, ReadBit(byte, 5));
		image[i * 4 + 1] = SetBit(image[i * 4 + 1], 0, ReadBit(byte, 4));
		image[i * 4 + 2] = SetBit(image[i * 4 + 2], 1, ReadBit(byte, 3));
		image[i * 4 + 2] = SetBit(image[i * 4 + 2], 0, ReadBit(byte, 2));
		image[i * 4 + 3] = SetBit(image[i * 4 + 3], 1, ReadBit(byte, 1));
		image[i * 4 + 3] = SetBit(image[i * 4 + 3], 0, ReadBit(byte, 0));
	}

	for (int i = 0; i < encodingDataLength; i++)
	{
		image[i * 4 + 0 + 16] = SetBit(image[i * 4 + 0 + 16], 1, ReadBit(encodingData[i], 7));
		image[i * 4 + 0 + 16] = SetBit(image[i * 4 + 0 + 16], 0, ReadBit(encodingData[i], 6));
		image[i * 4 + 1 + 16] = SetBit(image[i * 4 + 1 + 16], 1, ReadBit(encodingData[i], 5));
		image[i * 4 + 1 + 16] = SetBit(image[i * 4 + 1 + 16], 0, ReadBit(encodingData[i], 4));
		image[i * 4 + 2 + 16] = SetBit(image[i * 4 + 2 + 16], 1, ReadBit(encodingData[i], 3));
		image[i * 4 + 2 + 16] = SetBit(image[i * 4 + 2 + 16], 0, ReadBit(encodingData[i], 2));
		image[i * 4 + 3 + 16] = SetBit(image[i * 4 + 3 + 16], 1, ReadBit(encodingData[i], 1));
		image[i * 4 + 3 + 16] = SetBit(image[i * 4 + 3 + 16], 0, ReadBit(encodingData[i], 0));
	}

	int ret = SaveImage(outputFile, image, imageWidth, imageHeight, imageComp, FileExtension::FT_BMP);
	FreeImage(image);
	return ret;
}

int ImageEncoder::decode(const char* imageFile, const char* outputFile)
{
	int width = 0;
	int height = 0;
	int comp = 0;
	unsigned char* image = LoadImage(imageFile, width, height, comp);

	if (image == nullptr)
		return -1;

	int dataLength = 0;

	unsigned char split[4];
	for (int i = 0; i < 4; i++)
	{
		unsigned char byte = 0;
		byte |= ReadBit(image[i * 4 + 0], 1) << 7;
		byte |= ReadBit(image[i * 4 + 0], 0) << 6;
		byte |= ReadBit(image[i * 4 + 1], 1) << 5;
		byte |= ReadBit(image[i * 4 + 1], 0) << 4;
		byte |= ReadBit(image[i * 4 + 2], 1) << 3;
		byte |= ReadBit(image[i * 4 + 2], 0) << 2;
		byte |= ReadBit(image[i * 4 + 3], 1) << 1;
		byte |= ReadBit(image[i * 4 + 3], 0);


		split[i] = byte;
	}
	dataLength = (int)(split[3] << 24 | split[2] << 16 | split[1] << 8 | split[0]);

	std::fstream outFile(outputFile, std::ios::out | std::ios::binary);

	for (int i = 0; i < dataLength; i++)
	{
		unsigned char byte = 0;
		byte |= ReadBit(image[i * 4 + 0 + 16], 1) << 7;
		byte |= ReadBit(image[i * 4 + 0 + 16], 0) << 6;
		byte |= ReadBit(image[i * 4 + 1 + 16], 1) << 5;
		byte |= ReadBit(image[i * 4 + 1 + 16], 0) << 4;
		byte |= ReadBit(image[i * 4 + 2 + 16], 1) << 3;
		byte |= ReadBit(image[i * 4 + 2 + 16], 0) << 2;
		byte |= ReadBit(image[i * 4 + 3 + 16], 1) << 1;
		byte |= ReadBit(image[i * 4 + 3 + 16], 0);
		outFile.put((char)byte);
	}
	FreeImage(image);
	outFile.close();
	return 0;
}

unsigned char ImageEncoder::SetBit(unsigned char byte, int pos, int value)
{
	unsigned char result = (byte ^= (-(unsigned char)(value) ^ byte) & (1UL << pos));
	return result;
}

unsigned char ImageEncoder::ReadBit(unsigned char byte, int pos)
{
	return (byte >> pos) & 1;
}

int ImageEncoder::help()
{
	
	std::cout << "-e [file1] [file2] [file3]" << std::endl;
	std::cout << "\tEncodes the contents of [file1] into the two lowest order bits of image\n\t[file2] then saves the result in [file3]" << std::endl;
	std::cout << "\t[file2] and [file3] must be image types (BMP, TGA, PNG)" << std::endl;
	std::cout << "\n-d [file1] [file2]" << std::endl;
	std::cout << "\tDencodes the contents of [file1] into [file2]" << std::endl;
	std::cout << "\t[file1] should be the encoded image file of type (BMP, TGA, PNG)" << std::endl;
	std::cout << "\n-h shows this dialog" << std::endl;
	return 0;
}

unsigned char* ImageEncoder::LoadBinaryFile(const char* filename, int& size)
{
	std::fstream file(filename, std::ios::binary | std::ios::in);
	file.ignore(std::numeric_limits<std::streamsize>::max());
	std::streamsize fsize = file.gcount();
	file.clear();
	file.seekg(0, std::ios_base::beg);
	size = fsize;

	unsigned char* data = new unsigned char[size];

	file.read((char*)&data[0], size);

	return data;
}

unsigned char* ImageEncoder::LoadImage(const char* filename, int& width, int& height, int& comp)
{
	unsigned char* data = stbi_load(filename, &width, &height, &comp, 0);
	return data;
}

void ImageEncoder::FreeImage(void* image)
{
	stbi_image_free(image);
}

int ImageEncoder::SaveImage(const char* filename, unsigned char* data,
	int width, int height, int comp, FileExtension ft)
{
	std::fstream file(filename, std::ios::trunc);
	file.close();

	switch (ft)
	{
	case FileExtension::FT_BMP:
		stbi_write_bmp(filename, width, height, comp, data);
		break;
	case FileExtension::FT_PNG:
		stbi_write_png(filename, width, height, comp, data, comp);
		break;
	case FileExtension::FT_TGA:
		stbi_write_tga(filename, width, height, comp, data);
		break;
	default:
		std::cout << "Error saving file: " << filename << std::endl;
		return -1;
		break;
	}
	return 0;
}
