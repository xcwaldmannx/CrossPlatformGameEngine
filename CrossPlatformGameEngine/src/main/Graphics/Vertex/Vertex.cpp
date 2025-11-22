#include "Vertex.h"

using namespace ascen;

Vertex::Vertex(
	const VertexBinding& binding,
	const std::vector<VertexAttribute>& attributes) :
	mBinding(binding),
	mAttributes(attributes) {}

const VertexBinding& Vertex::getBinding() const
{
	return mBinding;
}

const std::vector<VertexAttribute>& Vertex::getAttributes() const
{
	return mAttributes;
}
