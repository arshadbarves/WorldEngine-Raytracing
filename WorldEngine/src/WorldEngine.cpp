#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"

#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

#include <glm/gtc/type_ptr.hpp>

namespace WorldEngine {

	class ExampleLayer : public Walnut::Layer
	{
	public:
		ExampleLayer()
			: m_Camera(45.0f, 0.1f, 100.0f)
		{
			Material& material = m_Scene.Materials.emplace_back();
			material.Albedo = glm::vec3(1.0f, 0.0f, 1.0f);
			material.Roughness = 0.0f;
			material.Metallic = 0.0f;
			material.EmissionColor = glm::vec3(1.0f, 1.0f, 1.0f);
			material.EmissionPower = 0.0f;

			Material& material2 = m_Scene.Materials.emplace_back();
			material2.Albedo = glm::vec3(0.2f, 0.3f, 1.0f);
			material2.Roughness = 0.1f;
			material2.Metallic = 0.0f;
			material2.EmissionColor = glm::vec3(1.0f, 1.0f, 1.0f);
			material2.EmissionPower = 0.0f;

			Material& material3 = m_Scene.Materials.emplace_back();
			material3.Albedo = glm::vec3(0.8f, 0.5f, 0.2f);
			material3.Roughness = 0.1f;
			material3.Metallic = 0.0f;
			material3.EmissionColor = material3.Albedo;
			material3.EmissionPower = 2.0f;

			{
				Sphere sphere;
				sphere.Position = glm::vec3(0.0f, 0.0f, 0.0f);
				sphere.Radius = 1.0f;
				sphere.MaterialIndex = 0;

				m_Scene.Spheres.push_back(sphere);
			}
			{
				Sphere sphere;
				sphere.Position = glm::vec3(2.0f, 0.0f, 0.0f);
				sphere.Radius = 1.0f;
				sphere.MaterialIndex = 2;

				m_Scene.Spheres.push_back(sphere);
			}
			{
				Sphere sphere;
				sphere.Position = glm::vec3(0.0f, -101.0f, 0.0f);
				sphere.Radius = 100.0f;
				sphere.MaterialIndex = 1;

				m_Scene.Spheres.push_back(sphere);
			}
		}

		virtual void OnUpdate(float ts) override
		{
			if(m_Camera.OnUpdate(ts))
				m_Renderer.ResetFrameIndex();
		}

		virtual void OnUIRender() override
		{
			ImGui::Begin("Scene Settings");
			for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
			{
				ImGui::PushID(i);
				Sphere& sphere = m_Scene.Spheres[i];
				ImGui::DragFloat3("Sphere Position", glm::value_ptr(sphere.Position), 0.1f);
				ImGui::DragFloat("Sphere Radius", &sphere.Radius, 0.1f);
				ImGui::DragInt("Sphere Material", &sphere.MaterialIndex, 1, 0, m_Scene.Materials.size() - 1);
				ImGui::Separator();

				ImGui::PopID();
			}

			for (size_t i = 0; i < m_Scene.Materials.size(); i++)
			{
				ImGui::PushID(i);
				Material& material = m_Scene.Materials[i];
				ImGui::Text("Material %d", i);
				ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
				ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Metallic", &material.Metallic, 0.01f, 0.0f, 1.0f);
				ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
				ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.1f, 0.0f, FLT_MAX);
				ImGui::Separator();

				ImGui::PopID();
			}

			ImGui::End();
			ImGui::Begin("Settings");
			ImGui::Text("Frame per second: %.1f", 1.0f / m_Renderer.GetLastRenderedTime());
			ImGui::Separator();
			ImGui::Text("Last frame time: %.3f ms", m_Renderer.GetLastRenderedTime() * 1000.0f);
			ImGui::Separator();
			ImGui::Text("Image size: %d x %d", m_ViewportWidth, m_ViewportHeight);
			ImGui::Separator();
			ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
			ImGui::Checkbox("Slow Random", &m_Renderer.GetSettings().SlowRandom);
			ImGui::Checkbox("Sky Light", &m_Renderer.GetSettings().EnableSky);
			ImGui::DragInt("Bounces", &m_Renderer.GetSettings().Bounces, 1.0f, 1, 1000);
			if (ImGui::Button("Reset Accumulation"))
			{
				m_Renderer.ResetFrameIndex();
			}
			ImGui::End();

			// Viewport
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			m_ViewportWidth = ImGui::GetContentRegionAvail().x;
			m_ViewportHeight = ImGui::GetContentRegionAvail().y;

			auto image = m_Renderer.GetFinalImage();

			if (image)
			{
				ImGui::Image(m_Renderer.GetFinalImage()->GetDescriptorSet(), { (float)m_Renderer.GetFinalImage()->GetWidth(), (float)m_Renderer.GetFinalImage()->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
			}
			ImGui::End();
			ImGui::PopStyleVar();

			Render();

			UI_DrawAboutModal();
		}

		void Render()
		{
			m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
			m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
			m_Renderer.Render(m_Scene, m_Camera);
		}

		void UI_DrawAboutModal()
		{
			if (!m_AboutModalOpen)
				return;

			ImGui::OpenPopup("About");
			m_AboutModalOpen = ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
			if (m_AboutModalOpen)
			{
				auto image = Walnut::Application::Get().GetApplicationIcon();
				ImGui::Image(image->GetDescriptorSet(), { 48, 48 });

				ImGui::SameLine();
				Walnut::UI::ShiftCursorX(20.0f);

				ImGui::BeginGroup();
				ImGui::Text("World Engine is a 3D Ray Tracer Render engine built on top of Vulkan.");
				ImGui::Text("Version 0.0.1");
				ImGui::Text("By Arshad Barves");

				ImGui::EndGroup();

				if (Walnut::UI::ButtonCentered("Close"))
				{
					m_AboutModalOpen = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void ShowAboutModal()
		{
			m_AboutModalOpen = true;
		}

	private:
		bool m_AboutModalOpen = false;

		Renderer m_Renderer;
		Camera m_Camera;
		Scene m_Scene;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "World Engine";
	spec.CustomTitlebar = true;

	Walnut::Application* app = new Walnut::Application(spec);
	std::shared_ptr<WorldEngine::ExampleLayer> exampleLayer = std::make_shared<WorldEngine::ExampleLayer>();
	app->PushLayer(exampleLayer);
	app->SetMenubarCallback([app, exampleLayer]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					exampleLayer->ShowAboutModal();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}