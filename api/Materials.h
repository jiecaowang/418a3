#pragma once

#include "Colour.h"
#include "BaseMaterials.h"

class air : public Material {
public:
	air();
};

class gold : public Material {
public:
	gold();
};

class jade : public Material {
public:
	jade();
};

class bronze : public Material {
public:
	bronze();
};

class glass : public Material {
public:
	glass();
};

class checkerBoard : public Material {
public:
	checkerBoard();
	Material* getMaterial(double s, double t);

private:
	Material Black;
	Material White;
};
