#include "scene.h"
#include "glm/glm.hpp"

namespace Wrench {
	void Scene::update(float delta_time) noexcept
	{
		[[maybe_unused]] auto _ = delta_time;
	}

	std::unique_ptr<Scene> Scene::load_scene() noexcept
	{
		std::unique_ptr<Scene> scene = std::make_unique<Scene>();
		std::vector<Vertex> verts{ 
			Vertex {glm::vec3( 0.5f, -0.5f, 0.0f), 1.0f, glm::vec3(1.0f,0.0f,0.0f), 0.0f},
			Vertex {glm::vec3(-0.5f, -0.5f, 0.0f), 0.0f, glm::vec3(0.0f,1.0f,0.0f), 0.0f},
			Vertex {glm::vec3( 0.0f,  0.5f, 0.0f), 0.5f, glm::vec3(0.0f,0.0f,1.0f), 1.0f},
		};
		std::vector<uint16_t> inds{ 0,1,2 };
		Model triangle(verts, inds);
		scene->m_models.push_back(triangle);
		return scene;
	}

	std::unique_ptr<Scene> Scene::load_scene([[maybe_unused]] std::filesystem::path &file_path) noexcept
	{
		std::unique_ptr<Scene> scene = std::make_unique<Scene>();
		return scene;
	}
};