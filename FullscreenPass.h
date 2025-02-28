#pragma once
#include "BindingPass.h"

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class InputLayout;
}

namespace Rgph {
	class FullscreenPass : public BindingPass
	{
	public:
		FullscreenPass(const std::string name, Graphics& gfx) noexcept(!IS_DEBUG);
		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) override;
	};
}