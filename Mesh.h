#pragma once
#include "Graphics.h"
#include "Drawable.h"

class Material;
class FrameCommander;
struct aiMesh;


class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTranform) const noexcept(!IS_DEBUG);
private:
	mutable DirectX::XMFLOAT4X4 transform;
};