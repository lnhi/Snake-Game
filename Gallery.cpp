#include "Gallery.h"

Gallery::Gallery(Painter& painter_)
    : painter(painter_)
{
    loadGamePictures();
}

Gallery::~Gallery()
{
    for (SDL_Texture* texture : pictures)
        SDL_DestroyTexture(texture);
}

void Gallery::loadGamePictures()
{
    pictures.push_back(painter.loadTexture("apple.png"));
    pictures.push_back(painter.loadTexture("snake_vertical.png"));
    pictures.push_back(painter.loadTexture("snake_horizontal.png"));
    pictures.push_back(painter.loadTexture("snake_head.png"));
    pictures.push_back(painter.loadTexture("snake_corner.png"));
    pictures.push_back(painter.loadTexture("snake_stub_hor.png"));
    pictures.push_back(painter.loadTexture("snake_stub_ver.png"));
    pictures.push_back(painter.loadTexture("box.png"));
}
