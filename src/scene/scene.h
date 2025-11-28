#ifndef SCENE_H
#define SCENE_H

#include <filesystem>
#include <memory>
#include <vector>
#include "model.h"

namespace Wrench {

	class Scene
	{
		std::vector<Model> m_models;
	public:
		void update(float delta_time) noexcept;

		static std::unique_ptr<Scene> load_scene() noexcept;
		static std::unique_ptr<Scene> load_scene(std::filesystem::path &file_path) noexcept;
	};


}; // namespace Wrench
#endif // !SCENE_H
