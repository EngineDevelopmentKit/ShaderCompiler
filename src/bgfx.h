/*
 * Copyright 2011-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#ifndef BGFX_H_HEADER_GUARD
#define BGFX_H_HEADER_GUARD

#include <stdarg.h> // va_list
#include <stdint.h> // uint32_t
#include <stdlib.h> // NULL

///
#define BGFX_HANDLE(_name) \
			struct _name { uint16_t idx; }; \
			inline bool isValid(_name _handle) { return bgfx::invalidHandle != _handle.idx; }

#define BGFX_INVALID_HANDLE { bgfx::invalidHandle }

namespace bx { struct AllocatorI; }

/// BGFX
namespace bgfx
{
	/// Fatal error enum.
	///
	/// @attention C99 equivalent is `bgfx_fatal_t`.
	///
	struct Fatal
	{
		enum Enum
		{
			DebugCheck,
			InvalidShader,
			UnableToInitialize,
			UnableToCreateTexture,
			DeviceLost,

			Count
		};
	};

	/// Renderer backend type enum.
	///
	/// @attention C99 equivalent is `bgfx_renderer_type_t`.
	///
	struct RendererType
	{
		/// Renderer types:
		enum Enum
		{
			Noop,         //!< No rendering.
			Direct3D9,    //!< Direct3D 9.0
			Direct3D11,   //!< Direct3D 11.0
			Direct3D12,   //!< Direct3D 12.0
			Gnm,          //!< GNM
			Metal,        //!< Metal
			OpenGLES,     //!< OpenGL ES 2.0+
			OpenGL,       //!< OpenGL 2.1+
			Vulkan,       //!< Vulkan

			Count
		};
	};

	/// Access mode enum.
	///
	/// @attention C99 equivalent is `bgfx_access_t`.
	///
	struct Access
	{
		/// Access:
		enum Enum
		{
			Read,
			Write,
			ReadWrite,

			Count
		};
	};

	/// Vertex attribute enum.
	///
	/// @attention C99 equivalent is `bgfx_attrib_t`.
	///
	struct Attrib
	{
		/// Corresponds to vertex shader attribute. Attributes:
		enum Enum
		{
			Position,  //!< a_position
			Normal,    //!< a_normal
			Tangent,   //!< a_tangent
			Bitangent, //!< a_bitangent
			Color0,    //!< a_color0
			Color1,    //!< a_color1
			Indices,   //!< a_indices
			Weight,    //!< a_weight
			TexCoord0, //!< a_texcoord0
			TexCoord1, //!< a_texcoord1
			TexCoord2, //!< a_texcoord2
			TexCoord3, //!< a_texcoord3
			TexCoord4, //!< a_texcoord4
			TexCoord5, //!< a_texcoord5
			TexCoord6, //!< a_texcoord6
			TexCoord7, //!< a_texcoord7

			Count
		};
	};

	/// Vertex attribute type enum.
	///
	/// @attention C99 equivalent is `bgfx_attrib_type_t`.
	///
	struct AttribType
	{
		/// Attribute types:
		enum Enum
		{
			Uint8,  //!< Uint8
			Uint10, //!< Uint10, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_UINT10`.
			Int16,  //!< Int16
			Half,   //!< Half, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_HALF`.
			Float,  //!< Float

			Count
		};
	};

	/// Uniform type enum.
	///
	/// @attention C99 equivalent is `bgfx_uniform_type_t`.
	///
	struct UniformType
	{
		/// Uniform types:
		enum Enum
		{
			Int1,
			End,

			Vec4,
			Mat3,
			Mat4,

			Count
		};
	};

	static const uint16_t invalidHandle = UINT16_MAX;

	BGFX_HANDLE(DynamicIndexBufferHandle);
	BGFX_HANDLE(DynamicVertexBufferHandle);
	BGFX_HANDLE(FrameBufferHandle);
	BGFX_HANDLE(IndexBufferHandle);
	BGFX_HANDLE(IndirectBufferHandle);
	BGFX_HANDLE(OcclusionQueryHandle);
	BGFX_HANDLE(ProgramHandle);
	BGFX_HANDLE(ShaderHandle);
	BGFX_HANDLE(TextureHandle);
	BGFX_HANDLE(UniformHandle);
	BGFX_HANDLE(VertexBufferHandle);
	BGFX_HANDLE(VertexDeclHandle);

	/// Memory release callback.
	///
	/// @attention C99 equivalent is `bgfx_release_fn_t`.
	///
	typedef void (*ReleaseFn)(void* _ptr, void* _userData);

	/// Memory obtained by calling `bgfx::alloc`, `bgfx::copy`, or `bgfx::makeRef`.
	///
	/// @attention C99 equivalent is `bgfx_memory_t`.
	///
	struct Memory
	{
		uint8_t* data;
		uint32_t size;
	};

	/// Uniform info.
	///
	/// @attention C99 equivalent is `bgfx_uniform_info_t`.
	///
	struct UniformInfo
	{
		char name[256];         //!< Uniform name.
		UniformType::Enum type; //!< Uniform type.
		uint16_t num;           //!< Number of elements in array.
	};

	/// Vertex declaration.
	///
	/// @attention C99 equivalent is `bgfx_vertex_decl_t`.
	///
	struct VertexDecl
	{
		VertexDecl();

		/// Start VertexDecl.
		///
		/// @attention C99 equivalent is `bgfx_vertex_decl_begin`.
		///
		VertexDecl& begin(RendererType::Enum _renderer = RendererType::Noop);

		/// End VertexDecl.
		///
		/// @attention C99 equivalent is `bgfx_vertex_decl_end`.
		///
		void end();

		/// Add attribute to VertexDecl.
		///
		/// @param[in] _attrib Attribute semantics. See: `bgfx::Attrib`
		/// @param[in] _num Number of elements 1, 2, 3 or 4.
		/// @param[in] _type Element type.
		/// @param[in] _normalized When using fixed point AttribType (f.e. Uint8)
		///   value will be normalized for vertex shader usage. When normalized
		///   is set to true, AttribType::Uint8 value in range 0-255 will be
		///   in range 0.0-1.0 in vertex shader.
		/// @param[in] _asInt Packaging rule for vertexPack, vertexUnpack, and
		///   vertexConvert for AttribType::Uint8 and AttribType::Int16.
		///   Unpacking code must be implemented inside vertex shader.
		///
		/// @remarks
		///   Must be called between begin/end.
		///
		/// @attention C99 equivalent is `bgfx_vertex_decl_add`.
		///
		VertexDecl& add(
			  Attrib::Enum _attrib
			, uint8_t _num
			, AttribType::Enum _type
			, bool _normalized = false
			, bool _asInt = false
			);

		/// Skip _num bytes in vertex stream.
		///
		/// @attention C99 equivalent is `bgfx_vertex_decl_skip`.
		///
		VertexDecl& skip(uint8_t _num);

		/// Decode attribute.
		///
		/// @attention C99 equivalent is ``.
		///
		void decode(
			  Attrib::Enum _attrib
			, uint8_t& _num
			, AttribType::Enum& _type
			, bool& _normalized
			, bool& _asInt
			) const;

		/// Returns true if VertexDecl contains attribute.
		bool has(Attrib::Enum _attrib) const { return UINT16_MAX != m_attributes[_attrib]; }

		/// Returns relative attribute offset from the vertex.
		uint16_t getOffset(Attrib::Enum _attrib) const { return m_offset[_attrib]; }

		/// Returns vertex stride.
		uint16_t getStride() const { return m_stride; }

		/// Returns size of vertex buffer for number of vertices.
		uint32_t getSize(uint32_t _num) const { return _num*m_stride; }

		uint32_t m_hash;
		uint16_t m_stride;
		uint16_t m_offset[Attrib::Count];
		uint16_t m_attributes[Attrib::Count];
	};

	/// Pack vec4 into vertex stream format.
	///
	/// @attention C99 equivalent is `bgfx_vertex_pack`.
	///
	void vertexPack(
		const float _input[4]
		, bool _inputNormalized
		, Attrib::Enum _attr
		, const VertexDecl& _decl
		, void* _data
		, uint32_t _index = 0
	);

	/// Unpack vec4 from vertex stream format.
	///
	/// @attention C99 equivalent is `bgfx_vertex_unpack`.
	///
	void vertexUnpack(
		float _output[4]
		, Attrib::Enum _attr
		, const VertexDecl& _decl
		, const void* _data
		, uint32_t _index = 0
	);

	/// Converts vertex stream data from one vertex stream format to another.
	///
	/// @param[in] _destDecl Destination vertex stream declaration.
	/// @param[in] _destData Destination vertex stream.
	/// @param[in] _srcDecl Source vertex stream declaration.
	/// @param[in] _srcData Source vertex stream data.
	/// @param[in] _num Number of vertices to convert from source to destination.
	///
	/// @attention C99 equivalent is `bgfx_vertex_convert`.
	///
	void vertexConvert(
		const VertexDecl& _destDecl
		, void* _destData
		, const VertexDecl& _srcDecl
		, const void* _srcData
		, uint32_t _num = 1
	);

	/// Weld vertices.
	///
	/// @param[in] _output Welded vertices remapping table. The size of buffer
	///   must be the same as number of vertices.
	/// @param[in] _decl Vertex stream declaration.
	/// @param[in] _data Vertex stream.
	/// @param[in] _num Number of vertices in vertex stream.
	/// @param[in] _epsilon Error tolerance for vertex position comparison.
	/// @returns Number of unique vertices after vertex welding.
	///
	/// @attention C99 equivalent is `bgfx_weld_vertices`.
	///
	uint16_t weldVertices(
		uint16_t* _output
		, const VertexDecl& _decl
		, const void* _data
		, uint16_t _num
		, float _epsilon = 0.001f
	);

} // namespace bgfx

#endif // BGFX_H_HEADER_GUARD
