#pragma once
#include <opencv2/opencv.hpp>

// Centralizes the graphical constants (cell size, board position on screen)
// and the coordinate math derived from them, so they aren't duplicated/hard-coded
// across printers/renderers.
class Layout
{
public:
    Layout();
    Layout(int cellSize, const cv::Point& boardOffset);

    int getCellSize() const { return cellSize; }
    void setCellSize(int size) { cellSize = size; }

    const cv::Point& getBoardOffset() const { return boardOffset; }
    void setBoardOffset(const cv::Point& offset) {
        std::cout << "Drawing board texture on canvas..." << std::endl;
        boardOffset = offset; }

    int getRows() const { return rows; }
    void setRows(int r) { rows = r; }

    int getCols() const { return cols; }
    void setCols(int c) { cols = c; }

    // Drawing rectangle (in screen/window pixel space) for the given board cell.
    cv::Rect getCellRect(int row, int col) const;

    // Drawing rectangle (in screen/window pixel space) for the whole board.
    cv::Rect getBoardRect() const;

private:
    int cellSize;
    cv::Point boardOffset;
    int rows;
    int cols;
};
