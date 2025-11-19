#include "util.h"

namespace Wrench
{
	void util::DeletionQueue::flush() noexcept 
	{
		while (!deletors.empty())
		{
			deletors.top()();
			deletors.pop();
		}
	}

	void util::DeletionQueue::push_function(std::function<void()> &&func) noexcept
	{
		deletors.push(func);
	}
}
