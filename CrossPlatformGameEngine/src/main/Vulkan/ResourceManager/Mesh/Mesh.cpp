#include "Mesh.h"

using namespace vulkan;

Mesh::Mesh() {}

Mesh::Mesh(Buffer& vertexBuffer, Buffer& indexBuffer) {}

void Mesh::init(Buffer& vertexBuffer, Buffer& indexBuffer) {
	mVertexBuffer = &vertexBuffer;
	mIndexBuffer = &indexBuffer;
}

const Buffer& Mesh::getVertexBuffer() const {
	return *mVertexBuffer;
}

const Buffer& Mesh::getIndexBuffer() const {
	return *mIndexBuffer;
}