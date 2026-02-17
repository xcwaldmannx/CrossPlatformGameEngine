#pragma once

#include <cstdint>

#include <string>
#include <vector>

namespace mass
{
	inline constexpr std::string_view API_VERSION = "1.1.0";

	inline constexpr std::string_view VERSION = "version";

	inline constexpr std::string_view NAME = "name";

	inline constexpr std::string_view VERTEX_LAYOUT = "vertexLayout";
	inline constexpr std::string_view VERTEX_STRIDE = "vertexStride";

	inline constexpr std::string_view VERTEX_OFFSET = "vertexOffset";
	inline constexpr std::string_view VERTEX_COUNT  = "vertexCount";

	inline constexpr std::string_view INDEX_OFFSET = "indexOffset";
	inline constexpr std::string_view INDEX_COUNT  = "indexCount";

	inline constexpr std::string_view TRANSFORM_OFFSET = "transformOffset";
	inline constexpr std::string_view TRANSFORM_COUNT  = "transformCount";

	inline constexpr std::string_view ANIMATION_OFFSET = "animationOffset";
	inline constexpr std::string_view ANIMATION_COUNT  = "animationCount";

	inline constexpr std::string_view VERTICES   = "vertices";
	inline constexpr std::string_view INDICES    = "indices";
	inline constexpr std::string_view TRANSFORMS = "transforms";
	inline constexpr std::string_view ANIMATIONS = "animations";
	inline constexpr std::string_view MESHES     = "meshes";

	inline constexpr std::string_view FLAGS          = "flags";
	inline constexpr std::string_view HAS_NORMALS    = "hasNormals";
	inline constexpr std::string_view HAS_COLORS     = "hasColors";
	inline constexpr std::string_view HAS_TEXCOORDS  = "hasTexCoords";
	inline constexpr std::string_view HAS_TRANSFORMS = "hasTransforms";
	inline constexpr std::string_view HAS_ANIMATIONS = "hasAnimations";

	struct VertexAttribute
	{
		size_t mComponentCount = 0;
		size_t mComponentSize = 0;
		size_t mOffset = 0;
	};

	struct VertexLayout
	{
		std::vector<VertexAttribute> mAttributes;
		size_t mStride = 0;
	};

	struct MeshLayout
	{
		std::string mName;
		size_t mVertexOffset    = 0;
		size_t mVertexCount     = 0;
		size_t mIndexOffset     = 0;
		size_t mIndexCount      = 0;
		size_t mTransformOffset = 0;
	};

	struct ModelLayout
	{
		VertexLayout mVertexLayout;
		std::vector<MeshLayout> mMeshLayouts;
		std::vector<float> mVertices;
		std::vector<uint32_t> mIndices;
		std::vector<float> mTransforms;
	};

	struct Configuration
	{
		VertexLayout mVertexLayout{};

		bool mHasNormals    = false;
		bool mHasColors     = false;
		bool mHasTexCoords  = false;
		bool mHasTransforms = false;
		bool mHasAnimations = false;
	};

}
