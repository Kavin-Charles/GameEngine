#pragma once

#include "Core.h"
#include "Window.h"
#include "Layer.h"
#include "LayerStack.h"
#include "Timestep.h"
#include "EngineContext.h"
#include "InputSystem.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/Renderer.h"
#include <memory>

namespace Engine {

	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }
		Renderer& GetRenderer() { return m_Renderer; }
		InputSystem& GetInput() { return *m_InputSystem; }
		EngineContext& GetContext() { return m_Context; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		void Close() { m_Running = false; }

		static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		std::unique_ptr<Window> m_Window;
		Renderer m_Renderer;
		std::unique_ptr<InputSystem> m_InputSystem;
		EngineContext m_Context;

		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		bool m_Running = true;
		bool m_Minimized = false;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

	Application* createApplication();
}
