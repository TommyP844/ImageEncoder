
#include "ImageEncoder.h"
#include <iostream>

// Return of 1 means there was a parameter error
// Return of -1 means there was a file opening error
// Return of 2 means there was an overflow error

int main(int argc, char* argv[])
{
	ImageEncoder* encoder = new ImageEncoder;

	if (strcmp(argv[1], "-e") == 0)
	{
		if (argc < 5 || argc > 5)
		{
			encoder->help();
			return 1;
		}
		else
			return encoder->encode(argv[2], argv[3], argv[4]);
	}
	else if (strcmp(argv[1], "-d") == 0)
	{
		if (argc < 4 || argc > 4)
		{
			encoder->help();
			return 1;
		}
		else
			return encoder->decode(argv[2], argv[3]);
	}
	else if (strcmp(argv[1], "-h") == 0)
	{
		encoder->help();
		if (argc > 2)
			return 1;
	}
	else
	{
		encoder->help();
		return 1;
	}

	delete encoder;

	return 0;
}