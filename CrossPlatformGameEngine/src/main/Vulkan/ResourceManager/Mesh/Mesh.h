#pragma once

#include "../Buffer/Buffer.h"

namespace vulkan {

	class Mesh {
	public:
		Mesh();
		Mesh(Buffer& vertexBuffer, Buffer& indexBuffer);

		void init(Buffer& vertexBuffer, Buffer& indexBuffer);

		const Buffer& getVertexBuffer() const;
		const Buffer& getIndexBuffer() const;

	private:
		Buffer* mVertexBuffer;
		Buffer* mIndexBuffer;
	};

}