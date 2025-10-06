#pragma once
#include <glad/glad.h>

class BlendManager {
public:

	enum BlendMode {
		NONE,
		ALPHA,
		ADDITIVE,
		MULTIPLY,
		SCREEN,
		PREMULTIPLIED
	};

	static void SetBlendMode(BlendMode mode) {

		switch (mode) {

			case NONE:
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);
				break;

			case ALPHA:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquation(GL_FUNC_ADD);
				glDepthMask(GL_FALSE);
				break;

			case ADDITIVE:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glBlendEquation(GL_FUNC_ADD);
				break;
			
			case MULTIPLY:
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				glBlendEquation(GL_FUNC_ADD);
				break;

			case SCREEN:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
				glBlendEquation(GL_FUNC_ADD);
				break;

			case PREMULTIPLIED:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquation(GL_FUNC_ADD);
				break;
		}

	}

};