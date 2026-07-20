#pragma once
#include "IBoardPrinter.h"
#include "img.hpp"
#include "Layout.h"
#include "TextureManager.h"
#include "BoardRenderer.h"
#include "PieceRenderer.h"

class GuiBoardPrinting : public IBoardPrinter
{
public:
    GuiBoardPrinting();

    void Convert(const GameSnapshot& snapshot) override;

private:
    Img boardImg;
    Layout layout;
    TextureManager textureManager;
    BoardRenderer boardRenderer;
    PieceRenderer pieceRenderer;
};
