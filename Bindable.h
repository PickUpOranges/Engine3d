#pragma once
#include <memory>
#include <string>
#include "GraphicsResource.h"


class Drawable;
class TechniqueProbe;
class Graphics;


namespace Bind {
	class Bindable : public GraphicsResource
	{
	public:
		virtual void Bind(Graphics& gfx) noexcept(!IS_DEBUG) = 0;
		virtual ~Bindable() = default;
		virtual void InitializeParentReference(const Drawable&) noexcept
		{}
		virtual std::string GetUID() const noexcept
		{
			assert(false);
			return "";
		}
		virtual void Accept(TechniqueProbe&)
		{}
	};


	class CloningBindable : public Bindable
	{
	public:
		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}
