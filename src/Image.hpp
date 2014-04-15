#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

namespace smadani {

	typedef unsigned char Byte;

	struct RGBPixel {
		Byte red, green, blue;
		RGBPixel() { }
		RGBPixel(Byte r, Byte g, Byte b) : red(r), green(g), blue(b)
		{
		}
		RGBPixel& operator= (RGBPixel const& rhs)
		{
			red = rhs.red;
			green = rhs.green;
			blue = rhs.blue;
			return *this;
		}
	};

	typedef RGBPixel ColorRGB;

	class Image {
	public:
		size_t width;
		size_t height;
		RGBPixel* pixels;

		Image() : width(0), height(0), pixels(NULL)
		{
		}

		Image(size_t w, size_t h) : width(w), height(h), pixels(new RGBPixel[width * height])
		{
		}
		Image(Image& rhs)
		{
			*this = rhs;
		}
		Image& allocate(size_t w, size_t h)
		{
			width = w;
			height = h;
			if (pixels == NULL) 
				pixels = new RGBPixel[width * height];
			return *this;
		}
		Image& operator= (Image const& rhs)
		{
			if (&rhs != this)
			{
				width = rhs.width;
				height = rhs.height;
				if (rhs.pixels != NULL)
				{
					if (pixels == NULL) pixels = new RGBPixel[width * height];
					for (register size_t i = 0; i < width * height; ++i)
						pixels[i] = rhs.pixels[i];
				}
			}
			return *this;
		}
		RGBPixel operator[](int i)
		{
			return pixels[i];
		}

		RGBPixel get_pixel(size_t x, size_t y)
		{
			return pixels[ (height - y) * width + x ];
		}

		void set_pixel(size_t x, size_t y, RGBPixel pxl)
		{
			pixels[ y * width + x ] = pxl;
		}
		~Image()
		{
			delete [] pixels;
		}

	private:
		
	};

	int load_TGA_file(std::string file_name, Image& image)
	{
		FILE* tga_file = fopen(file_name.c_str(), "rb");
		if (tga_file == NULL) return -1;
		
		Byte tga_header[18];
		fread(tga_header, sizeof(Byte), 18, tga_file);

		//Checking for Uncompressed True color Image Type
		if (tga_header[2] != 2)
		{
			std::cout << "The image isn't uncompressed true color" << std::endl;
			fclose(tga_file);
			return -1;
		}

		//Checking for 24-bits / pixel 
		if (tga_header[16] != 24)
		{
			std::cout << "It's not a 24-bits / pixel TGA image" << std::endl;
			fclose(tga_file);
			return -1;
		}

		size_t width = (size_t) ( (size_t)tga_header[12] & 0xFF | (size_t)tga_header[13] * 256);
		size_t height = (size_t) ( (size_t)tga_header[14] & 0xFF | (size_t)tga_header[15] * 256);

		std::cout << width << ", " << height << std::endl;
		image.allocate(width, height);
		Byte pixel_colors[3];

		for (size_t i = 0; i < image.height; ++i) {
			if (tga_header[17] & 0x20)
			{
				//Image is Inverted horizontally
				for (size_t j = 0; j < image.width; j++)
				{
					fread(pixel_colors, 3 * sizeof(Byte), 1, tga_file);
					RGBPixel& curr_pixel = image.pixels[ (height - 1 - i) * width + j];
					curr_pixel.blue = pixel_colors[0];
					curr_pixel.green = pixel_colors[1];
					curr_pixel.red = pixel_colors[2];
				}
			}
			else {
				//Image is Normal
				for (size_t j = 0; j < image.width; j++)
				{
					fread(pixel_colors, 3 * sizeof(Byte), 1, tga_file);
					RGBPixel& curr_pixel = image.pixels[i * width + j];
					curr_pixel.blue = pixel_colors[0];
					curr_pixel.green = pixel_colors[1];
					curr_pixel.red = pixel_colors[2];
				}			
			}
		}

		fclose(tga_file);
		return 1;
	}


	GLuint load_texture(Image& image) {

		GLuint texture_id;
		glGenTextures(1, &texture_id);				//Make room for our texture
		glBindTexture(GL_TEXTURE_2D, texture_id);  //Tell OpenGL which texture to edit

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//Map the image to the texture	
		glTexImage2D(GL_TEXTURE_2D,					//Always GL_TEXTURE_2D
					 0,								//0 for now
					 GL_RGB,						//Format OpenGL uses for image
					 image.width, image.height,		//Width and height
					 0,								//The border of the image
					 GL_RGB,						//GL_RGB, because pixels are stored in RGB format
					 GL_UNSIGNED_BYTE,				//GL_UNSIGNED_BYTE, because pixels are stored
													//as unsigned numbers
					 image.pixels);					//The actual pixel data
		return texture_id;							//Returns the id of the texture
	}

}

#endif