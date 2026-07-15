#pragma once
class Position
{
	int row;
	int col;

public:
	Position();
	Position(int row, int col);
	int getRow() const;
	int getCol() const;
	void setRow(int row);
	void setCol(int col);
	void operator=(const Position& other);
	bool operator==(const Position& other) const;
	bool operator!=(const Position& other) const;
};

