#pragma once

#include <vector>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "GL/glew.h"

using namespace std;

GLuint Create1DTexture(vector<float> d, GLenum format);