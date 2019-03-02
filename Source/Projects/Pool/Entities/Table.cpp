#include "Table.h"

using namespace PoolObjects;

Table::Table() {}

Table::Table(glm::vec3 position, Cushion **cushions, Pocket **pockets)
    : Entity(position), cushions(cushions), pockets(pockets) {}

Table::~Table() {}

Cushion** Table::GetCushions() {
    return cushions;
}
Pocket** Table::GetPockets() {
    return pockets;
}