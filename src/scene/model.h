#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "glm/glm.hpp"

namespace Wrench
{
    struct Vertex
    {
        glm::vec3 position;
        float uv_x;
        glm::vec3 color;
        float uv_y;
    };

    class Model
    {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices; // TODO: are 16bit indices ok?
    public:
        Model(std::vector<Vertex> &vertices, std::vector<uint16_t> &indices);
    };

};

#endif // !MODEL_H
