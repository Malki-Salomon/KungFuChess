#include "pch.h"
#include "Position.h"

Position::Position() : row(-1), col(-1) {}
Position::Position(int row, int col) : row(row), col(col) {}
int Position::getRow() const { return row; }
int Position::getCol() const { return col; }
void Position::setRow(int row) { this->row = row; }
void Position::setCol(int col) { this->col = col; }
void Position::operator=(const Position& other)
{
	row = other.row;
	col = other.col;
}
bool Position::operator==(const Position& other) const
{
	return row == other.row && col == other.col;
}
bool Position::operator!=(const Position& other) const
{
	return !(*this == other);
}
