#include "Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

class Sandbox : public Engine::Application {
public:
	Sandbox()
	{
		// Create the engine editor layer
		auto* editorLayer = new Engine::EditorLayer();
		PushLayer(editorLayer);

		// --- User scene setup ---
		auto scene = std::make_shared<Engine::Scene>();

		// Shader
		auto shader = Engine::Shader::CreateFromFile("PhongShader", "assets/shaders/phong.vert", "assets/shaders/phong.frag");
		if (!shader)
		{
			ENGINE_LOG_ERROR("FATAL: Failed to load shader!");
		}

		// Meshes
		auto sphereMesh = Engine::Mesh::CreateSphere(1.0f);
		auto cubeMesh = Engine::Mesh::CreateCube(1.0f);
		auto planeMesh = Engine::Mesh::CreatePlane(10.0f);

		// Textures
		auto checkerboardTexture = Engine::Texture2D::Create("assets/textures/checkerboard.png");

		// --- Entities ---
		auto ground = scene->CreateEntity("Ground");
		ground.GetComponent<Engine::TransformComponent>().TransformData.Position = { 0.0f, -0.5f, 0.0f };
		auto& groundMr = ground.AddComponent<Engine::MeshRendererComponent>(planeMesh, glm::vec4{ 0.4f, 0.4f, 0.4f, 1.0f });
		groundMr.MaterialData.AlbedoTexture = checkerboardTexture;

		auto greenCube = scene->CreateEntity("Green Cube");
		greenCube.GetComponent<Engine::TransformComponent>().TransformData.Position = { -1.5f, 0.5f, 0.0f };
		greenCube.AddComponent<Engine::MeshRendererComponent>(cubeMesh, glm::vec4{ 0.2f, 0.8f, 0.3f, 1.0f });

		auto redSphere = scene->CreateEntity("Red Sphere");
		redSphere.GetComponent<Engine::TransformComponent>().TransformData.Position = { 1.5f, 0.5f, 0.0f };
		redSphere.AddComponent<Engine::MeshRendererComponent>(sphereMesh, glm::vec4{ 0.8f, 0.2f, 0.2f, 1.0f });

		auto blueCube = scene->CreateEntity("Blue Cube");
		blueCube.GetComponent<Engine::TransformComponent>().TransformData.Position = { 0.0f, 1.5f, 0.0f };
		blueCube.GetComponent<Engine::TransformComponent>().TransformData.Scale = { 0.5f, 0.5f, 0.5f };
		blueCube.AddComponent<Engine::MeshRendererComponent>(cubeMesh, glm::vec4{ 0.2f, 0.2f, 0.8f, 1.0f });

		// Game Camera
		auto gameCam = scene->CreateEntity("Game Camera");
		gameCam.GetComponent<Engine::TransformComponent>().TransformData.Position = { 3.0f, 2.0f, 3.0f };
		gameCam.GetComponent<Engine::TransformComponent>().TransformData.Rotation = { -20.0f, -135.0f, 0.0f };
		auto& gameCamComp = gameCam.AddComponent<Engine::CameraComponent>();
		gameCamComp.CameraData.SetPerspective(60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
		gameCamComp.IsGameCamera = true;

		// Hand off to editor
		editorLayer->SetScene(scene, shader);
	}
};

Engine::Application* Engine::createApplication() {
	return new Sandbox();
}
