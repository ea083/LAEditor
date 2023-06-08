#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class PickingTexture {
public:
	PickingTexture() {}
	~PickingTexture() {
		if (FBO != 0) {
			glDeleteFramebuffers(1, &FBO);
		}

		if (pickingTexture != 0) {
			glDeleteTextures(1, &pickingTexture);
		}

		if (depthTexture != 0) {
			glDeleteTextures(1, &depthTexture);
		}
	}
	void init(unsigned int WindowWidth, unsigned int WindowHeight)
	{
		// Create the FBO
		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		// Create the texture object for the primitive information buffer
		glGenTextures(1, &pickingTexture);
		glBindTexture(GL_TEXTURE_2D, pickingTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, WindowWidth, WindowHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTexture, 0);

		// Create the texture object for the depth buffer
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		// Verify that the FBO is correct
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
			exit(1);
		}

		// Restore the default framebuffer
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void enableWriting() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	}

	void disableWriting() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	struct PixelInfo {
		unsigned int ObjectID = 0;
		unsigned int DrawID = 0;
		unsigned int PrimID = 0;

		void print() {
			std::cout << "Object " << ObjectID << " draw " << DrawID << " prim " << "PrimID" << std::endl;
		}
	};

	PixelInfo readPixel(unsigned int x, unsigned int y) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);

		glReadBuffer(GL_COLOR_ATTACHMENT0);

		PixelInfo Pixel;
		glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return Pixel;
	}
private:
	unsigned int FBO = 0, pickingTexture = 0, depthTexture = 0;
};