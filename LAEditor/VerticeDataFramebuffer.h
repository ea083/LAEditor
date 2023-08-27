#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Utilities.h"
#include <iostream>




class VerticeDataFramebuffer {
public:

	VerticeDataFramebuffer() : FBO(0), RBO(0), textureID(0) {}

	void initVerticeDataFramebuffer(int width, int height) {
		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::VERTICEDATAFRAMEBUFFER:: Framebuffer is not complete!\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void bindFramebuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	}

	void unbindFramebuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void rescaleFramebuffer(float width, float height) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)width, (GLsizei)height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}

	Utilities::PixelData getDataAtPixel(int x, int y, float width, float height, float windowWidth, float windowHeight) {
		//x = x - width + viewerXPos; 
		x = (int)(x * (windowWidth / width) - 14);
		y = (int)(y * (windowHeight / height) - 50);
		y = (int)(windowHeight - y);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		std::vector<float> pixelData((const unsigned __int64)(width * height * 3));
		glBindTexture(GL_TEXTURE_2D, textureID);
		glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pixelData.data());
		glBindTexture(GL_TEXTURE_2D, 0);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		/*std::cout << "Pixel at (" << x << ", " << y << "): R = " << (pixelData[0])
			<< ", G=" << (pixelData[1])
			<< ", B=" << (pixelData[2]) << std::endl;*/
		Utilities::PixelData returnVal;
		returnVal.r = pixelData[0];
		returnVal.g = pixelData[1];
		returnVal.b = pixelData[2];
		return returnVal;
	}

	void cleanUp() {
		glDeleteFramebuffers(1, &FBO);
		glDeleteTextures(1, &textureID);
		glDeleteRenderbuffers(1, &RBO);
	}

	unsigned int getTextureID() {
		return textureID;
	}

private:
	unsigned int FBO, textureID, RBO;
};