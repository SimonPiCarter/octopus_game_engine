#include "RenderTexture.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace cuttlefish
{
RenderTexture::RenderTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

RenderTexture::~RenderTexture()
{
	//Deallocate
	free();
}

bool RenderTexture::createBlank( int width, int height, SDL_Renderer* renderer_p )
{
	//Get rid of preexisting rendertexture
	free();

    //Create uninitialized texture
    mTexture = SDL_CreateTexture( renderer_p, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height );
    if( mTexture == NULL )
    {
        printf( "Unable to create streamable blank texture! SDL Error: %s\n", SDL_GetError() );
    }
    else
    {
        mWidth = width;
        mHeight = height;
    }

	return mTexture != NULL;
}

void RenderTexture::free()
{
	//Free rendertexture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void RenderTexture::render(SDL_Renderer* renderer, int x, int y, int h, int w, SDL_Rect* clip ) const
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, w, h };

	SDL_RenderCopy( renderer, mTexture, clip, &renderQuad );

}

void RenderTexture::startRendering(SDL_Renderer* renderer_p)
{
    SDL_SetRenderTarget( renderer_p, mTexture );
}

void RenderTexture::stopRendering(SDL_Renderer* renderer_p) const
{
    SDL_SetRenderTarget( renderer_p, nullptr );
}

int RenderTexture::getWidth()
{
	return mWidth;
}

int RenderTexture::getHeight()
{
	return mHeight;
}

} // namespace cuttlefish

