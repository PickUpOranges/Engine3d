#include "BindingPass.h"
#include "Bindable.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RenderGraphCompileException.h"

namespace Rgph {
	BindingPass::BindingPass(std::string name, std::vector<std::shared_ptr<Bind::Bindable>> binds)
		:
		Pass(std::move(name)),
		binds(std::move(binds))
	{}

	void BindingPass::AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept
	{
		binds.push_back(std::move(bind));
	}

	void BindingPass::BindAll(Graphics& gfx) const noexcept
	{
		for (auto& bind : binds)
		{
			bind->Bind(gfx);
		}
		BindBufferResources(gfx);
	}

	void BindingPass::Finalize()
	{
		Pass::Finalize();
		if (!renderTarget && !depthStencil)
		{
			throw RGC_EXCEPTION("BindingPass [" + GetName() + "] needs at least one of a renderTarget or depthStencil");
		}
	}
}