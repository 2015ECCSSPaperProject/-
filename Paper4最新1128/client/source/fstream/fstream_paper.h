
#pragma once

// fstream �g��

#include <fstream>

struct Vector3;

std::fstream& operator >> (std::fstream&, Vector3&);
std::ifstream& operator >> (std::ifstream&, Vector3&);
