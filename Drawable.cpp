#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "BindableBase.h"
#include "BindableCodex.h"
#include <assimp/scene.h>
#include "Material.h"

using namespace Bind;

void Drawable::Submit(size_t channelFilter) const noexcept
{
	for (const auto& tech : techniques)
	{
		tech.Submit(*this, channelFilter);
	}
}

Drawable::Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept
{
	pVertices = mat.MakeVertexBindable(gfx, mesh,scale);
	pIndices = mat.MakeIndexBindable(gfx, mesh);
	pTopology = Bind::Topology::Resolve(gfx);

	for (auto& t : mat.GetTechniques())
	{
		AddTechnique(std::move(t));
	}
}

void Drawable::AddTechnique(Technique tech_in) noexcept
{
	tech_in.InitializeParentReferences(*this);
	techniques.push_back(std::move(tech_in));
}

void Drawable::Bind(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	pTopology->Bind(gfx);
	pIndices->Bind(gfx);
	pVertices->Bind(gfx);
}

UINT Drawable::GetIndexCount() const noexcept(!IS_DEBUG)
{
	return pIndices->GetCount();
}

Drawable::~Drawable()
{}


void Drawable::Accept(TechniqueProbe& probe)
{
	for (auto& t : techniques)
	{
		t.Accept(probe);
	}
}

void Drawable::LinkTechniques(Rgph::RenderGraph& rg)
{
	for (auto& tech : techniques)
	{
		tech.Link(rg);
	}
}