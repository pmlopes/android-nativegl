#include <malloc.h>

#include "namodel.h"

void naDrawModel(nModel *model) {
	if(model->vertex) {
		glVertexPointer(3, GL_FIXED, 0, model->vertex);
		glEnableClientState(GL_VERTEX_ARRAY);
	}
	if(model->normal) {
		glNormalPointer(GL_FIXED, 0, model->normal);
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	if(model->uv) {
		glTexCoordPointer(2, GL_FIXED, 0, model->uv);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, model->texId);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDrawElements(GL_TRIANGLES, model->index_len, GL_UNSIGNED_SHORT, model->index);
}

void naDrawModelGroup(nModel *model, int bindBuffers, int groupId) {
	if(bindBuffers) {
		glVertexPointer(3, GL_FIXED, 0, model->vertex);
		glEnableClientState(GL_VERTEX_ARRAY);
		if(model->normal) {
			glNormalPointer(GL_FIXED, 0, model->normal);
			glEnableClientState(GL_NORMAL_ARRAY);
		}
		if(model->uv && model->texId) {
			glTexCoordPointer(2, GL_FIXED, 0, model->uv);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindTexture(GL_TEXTURE_2D, model->texId);
		} else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	int startIdx = model->group[groupId*2];
	int endIdx = model->group[groupId*2+1];

	glDrawElements(GL_TRIANGLES, (endIdx - startIdx), GL_UNSIGNED_SHORT, model->index + startIdx);
}

void naFreeModel(nModel *model) {
	if(model) {
		model->group_len = 0;
		if(model->group) {
			free(model->group);
			model->group = NULL;
		}
		model->index_len = 0;
		if(model->index) {
			free(model->index);
			model->index = NULL;
		}
		if(model->vertex) {
			free(model->vertex);
			model->vertex = NULL;
		}
		if(model->normal) {
			free(model->normal);
			model->normal = NULL;
		}
		if(model->uv) {
			free(model->uv);
			model->uv = NULL;
		}
		glDeleteTextures(1, &model->texId);
		model->texId = 0;

		model = NULL;
	}
}
