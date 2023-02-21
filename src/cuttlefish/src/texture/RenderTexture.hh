#ifndef __RenderTexture__
#define __RenderTexture__

#include <string>

struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;

namespace cuttlefish
{
//RenderTexture wrapper class
class RenderTexture
{
	public:
		//Initializes variables
		RenderTexture();

		//Deallocates memory
		~RenderTexture();

		//Loads image at specified path
		bool createBlank( int width, int height, SDL_Renderer* renderer_p );

		//Deallocates rendertexture
		void free();

		//Renders rendertexture at given point
		void render( SDL_Renderer* renderer, int x, int y, int h, int w, SDL_Rect* clip = nullptr ) const;

        /// @brief start rendering into the texture
        /// @param renderer_p
        void startRendering(SDL_Renderer* renderer_p);

        /// @brief stop rendering into the texture
        /// @param renderer_p
        void stopRendering(SDL_Renderer* renderer_p) const;

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware rendertexture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

} // namespace cuttlefish


#endif
