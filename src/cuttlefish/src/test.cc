#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture/LTexture.hh"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace cuttlefish;

/// cf
/// http://lazyfoo.net/tutorials/SDL/index.php#The%20Viewport
///

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;

bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{

    //Loading success flag
    bool success = true;

    //Load Foo' texture
    if( !gFooTexture.loadFromFile( "resources/circle.png", gRenderer ) )
    {
        printf( "Failed to load Foo' texture image!\n" );
        success = false;
    }

    //Load background texture
    if( !gBackgroundTexture.loadFromFile( "resources/wp3386769.jpg", gRenderer ) )
    {
        printf( "Failed to load background texture image!\n" );
        success = false;
    }

    return success;
}

void close()
{
    //Free loaded images
    gFooTexture.free();
    gBackgroundTexture.free();

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    SDL_Rect gSpriteClips[ 4 ];

    //Set top left sprite
    gSpriteClips[ 0 ].x =   0;
    gSpriteClips[ 0 ].y =   0;
    gSpriteClips[ 0 ].w = 64;
    gSpriteClips[ 0 ].h = 64;

    //Set top right sprite
    gSpriteClips[ 1 ].x = 64;
    gSpriteClips[ 1 ].y =   0;
    gSpriteClips[ 1 ].w = 64;
    gSpriteClips[ 1 ].h = 64;

    //Set bottom left sprite
    gSpriteClips[ 2 ].x =   0;
    gSpriteClips[ 2 ].y = 64;
    gSpriteClips[ 2 ].w = 64;
    gSpriteClips[ 2 ].h = 64;

    //Set bottom right sprite
    gSpriteClips[ 3 ].x = 64;
    gSpriteClips[ 3 ].y = 64;
    gSpriteClips[ 3 ].w = 64;
    gSpriteClips[ 3 ].h = 64;
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            bool quit = false;

            //Event handler
            SDL_Event e;
                       //While application is running
            while( !quit )
            {
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                }

                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );

                //Render background texture to screen
                gBackgroundTexture.render(gRenderer, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH );

                //Render top left sprite
                gFooTexture.render( gRenderer, 0, 0, gSpriteClips[ 0 ].h*2, gSpriteClips[ 0 ].w*2,
                    &gSpriteClips[ 0 ] );

                //Render top right sprite
                gFooTexture.render( gRenderer, SCREEN_WIDTH - gSpriteClips[ 1 ].w, 0, gSpriteClips[ 0 ].h, gSpriteClips[ 0 ].w,
                    &gSpriteClips[ 1 ] );

                //Render bottom left sprite
                gFooTexture.render( gRenderer, 0, SCREEN_HEIGHT - gSpriteClips[ 2 ].h, gSpriteClips[ 0 ].h, gSpriteClips[ 0 ].w,
                    &gSpriteClips[ 2 ] );

                //Render bottom right sprite
                gFooTexture.render( gRenderer, SCREEN_WIDTH - gSpriteClips[ 3 ].w, SCREEN_HEIGHT - gSpriteClips[ 3 ].h, gSpriteClips[ 0 ].h, gSpriteClips[ 0 ].w,
                    &gSpriteClips[ 3 ] );

                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}