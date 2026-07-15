#include "pch.h"
#include "BoardMapper.h"

Position BoardMapper::pixelToCell(int x, int y) {
	return Position(x / PIXEL, y / PIXEL);
}
