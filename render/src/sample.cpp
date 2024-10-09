#include <sample.h>

namespace render
{
    Sample::Sample(uint32_t width, uint32_t height, std::string title, const DXGI_FORMAT rtFormat, const DXGI_FORMAT dsFormat) :
        m_width(width),
        m_height(height),
        m_title(title),
        m_viewport({0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 1.0f}),
        m_scissorRect({0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height)})
    {
        m_pWindow = make_unique<Window>(this, m_width, m_height, m_title);
        m_pDevice = make_unique<Device>();
        m_pCommander = make_unique<Commander>(*m_pDevice);
        m_pSwapchain = make_unique<Swapchain>(*m_pWindow, *m_pDevice, *m_pCommander, rtFormat, dsFormat);

        m_pDescriptor = new Descriptor(*m_pDevice);
        m_pPipeline = new Pipeline(*m_pSwapchain);
    }

    Sample::Sample(uint32_t width, uint32_t height, std::string title, bool depthEnable) :
        Sample(width, height, title, DXGI_FORMAT_B8G8R8A8_UNORM, depthEnable ? DXGI_FORMAT_D32_FLOAT : DXGI_FORMAT_UNKNOWN)
    {
    }

    Sample::Sample(uint32_t width, uint32_t height, std::string title) :
        Sample(width, height, title, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_UNKNOWN)
    {
    }

    Sample::Sample() :
        Sample(800, 600, "Sample Application")
    {
    }

    Sample::~Sample()
    {
        if(m_pDescriptor) delete m_pDescriptor;
        if(m_pPipeline) delete m_pPipeline;
    }

    void Sample::WaitForPreviousFrame()
    {
        m_pCommander->WaitForPreviousFrame();
        m_pSwapchain->UpdateFrameIndex();
    }

    void Sample::TransitionPresentToRenderTarget()
    {
        D3D12_RESOURCE_BARRIER rtBarrier;
        m_pSwapchain->TransitionBarrierPresentToRenderTarget(rtBarrier);
        m_pCommander->GetCommandList()->ResourceBarrier(1, &rtBarrier);
    }

    void Sample::TransitionRenderTargetToPrsent()
    {
        D3D12_RESOURCE_BARRIER presentBarrier;
        m_pSwapchain->TransitionBarrierRenderTargetToPresent(presentBarrier);
        m_pCommander->GetCommandList()->ResourceBarrier(1, &presentBarrier);
    }
}