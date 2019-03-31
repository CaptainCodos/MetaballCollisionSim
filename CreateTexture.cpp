#include "CreateTexture.h"

using namespace std;

GLuint Create1DTexture(vector<float> d, GLenum format)
{
	GLuint textureId_;

	// generate the specified number of texture objects 
	glGenTextures(1, &textureId_);
	//assert(glGetError() == GL_NO_ERROR);

	// bind texture
	glBindTexture(GL_TEXTURE_1D, textureId_);
	//assert(glGetError() == GL_NO_ERROR);

	// tells OpenGL how the data that is going to be uploaded is aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//assert(glGetError() == GL_NO_ERROR);

	/*float data[] =
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};*/

	float* data = d.data();

	glTexImage1D(
		GL_TEXTURE_1D,      // Specifies the target texture. Must be GL_TEXTURE_1D or GL_PROXY_TEXTURE_1D.
		0,                  // Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
		GL_RGBA32F,
		d.size() / sizeof(d[0]),
		0,                  // border: This value must be 0.
		format,
		GL_FLOAT,
		data
	);
	//assert(glGetError() == GL_NO_ERROR);

	// texture sampling/filtering operation.
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//assert(glGetError() == GL_NO_ERROR);

	glBindTexture(GL_TEXTURE_1D, 0);
	//assert(glGetError() == GL_NO_ERROR);

	return textureId_;
}