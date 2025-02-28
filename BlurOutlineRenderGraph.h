#pragma once
#include "RenderGraph.h"
#include <memory>
#include "ConstantBuffersEx.h"


class Graphics;
class Camera;
namespace Bind
{
	class Bindable;
	class RenderTarget;
	class ShadowSampler;
	class ShadowRasterizer;
}

namespace Rgph {
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph(Graphics& gfx);
		void RenderWindows(Graphics& gfx);
		void BindMainCamera(Camera& cam);
		void BindShadowCamera(Camera& cam);
		void DumpShadowMap(Graphics& gfx, const std::string& path);
	private:
		void RenderKernelWindow(Graphics& gfx);
		// private functions
		void SetKernelGauss(int radius, float sigma) noexcept(!IS_DEBUG);
		void SetKernelBox(int radius) noexcept(!IS_DEBUG);
		// private data
		enum class KernelType
		{
			Gauss,
			Box,
		} kernelType = KernelType::Gauss;
		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
	};
}
