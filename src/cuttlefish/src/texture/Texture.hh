#ifndef __Texture__
#define __Texture__

#include <string>

struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;

namespace cuttlefish
{
//Texture wrapper class
class Texture
{
	public:
		//Initializes variables
		Texture();

		//Deallocates memory
		~Texture();

		//Loads image at specified path
		bool loadFromFile( std::string path, SDL_Renderer* renderer );

		//Deallocates texture
		void free();

		//Renders texture at given point
		void render( SDL_Renderer* renderer, int x, int y, int h, int w, SDL_Rect* clip = nullptr ) const;

		//Gets image dimensions
		int getWidth() const;
		int getHeight() const;

	private:
		std::string _path;

		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

} // namespace cuttlefish


#endif
