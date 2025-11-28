#include "model.h"

namespace Wrench
{
	Model::Model(std::vector<Vertex> &vertices, std::vector<uint16_t> &indices) 
		: vertices(vertices), indices(indices)
	{ 
	}
};