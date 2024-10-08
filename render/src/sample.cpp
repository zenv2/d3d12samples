#include <sample.h>

namespace render
{
    Sample::Sample(uint32_t width, uint32_t height, std::string title) :
        m_width(width),
        m_height(height),
        m_title(title),
        m_viewport({0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height), .1f, 1000.f}),
        m_scissorRect({0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height)})
    {
        m_pWindow = make_unique<Window>(this, m_width, m_height, m_title);
        m_pDevice = make_unique<Device>();
        m_pCommander = make_unique<Commander>(*m_pDevice);
        m_pSwapchain = make_unique<Swapchain>(*m_pWindow, *m_pDevice, *m_pCommander);

        m_pDescriptor = new Descriptor(*m_pDevice);
        m_pPipeline = new Pipeline();
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
}