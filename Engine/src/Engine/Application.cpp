#include "Application.h"
#include "Log.h"

#include <GLFW/glfw3.h>
#include <filesystem>

namespace Engine {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		ENGINE_LOG_INFO("Working directory: {0}", std::filesystem::current_path().string());

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback([this](Event& e) { this->OnEvent(e); });

		// Initialize subsystems
		m_Renderer.Init();
		m_InputSystem = std::make_unique<InputSystem>(*m_Window);

		// Build context
		m_Context.RendererInstance = &m_Renderer;
		m_Context.Input = m_InputSystem.get();
		m_Context.WindowInstance = m_Window.get();

		// Create and push ImGui as an overlay
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		m_Renderer.Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(e); });
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) { return OnWindowResize(e); });

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			// ImGui render pass
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		m_Renderer.SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		return false;
	}
}
