#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>
#include "Technique.h"

class TechniqueProbe;
class Material;
struct aiMesh;

namespace Rgph {
	class RenderGraph;
}

namespace Bind
{ 
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void AddTechnique(Technique tech_in) noexcept;
	void Submit(size_t channelFilter) const noexcept;
	void Bind(Graphics& gfx) const noexcept(!IS_DEBUG);
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount() const noexcept(!IS_DEBUG);
	void LinkTechniques(Rgph::RenderGraph&);
	virtual ~Drawable();

protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};
