#include "Layout.h"

Layout::Layout()
    : cellSize(100), boardOffset(0, 0), rows(8), cols(8)
{
}

Layout::Layout(int cellSize, const cv::Point& boardOffset)
    : cellSize(cellSize), boardOffset(boardOffset), rows(8), cols(8)
{
}

cv::Rect Layout::getCellRect(int row, int col) const
{
    return cv::Rect(
        boardOffset.x + col * cellSize,
        boardOffset.y + row * cellSize,
        cellSize,
        cellSize);
}

cv::Rect Layout::getBoardRect() const
{
    return cv::Rect(
        boardOffset.x,
        boardOffset.y,
        cols * cellSize,
        rows * cellSize);
}
