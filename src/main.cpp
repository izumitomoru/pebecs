#include "pebecs.h"

// argc is argument count, char buffer is argument
int main(int argc, char*[]) {
  using namespace pebecs;
  using Entity = uint32_t;

  entt::basic_registry<Entity> r;
  Entity                       e = r.create();
  r.emplace<Transform>(e);

  Registry<Entity> reg;

  Entity e1 = reg.create();
  Entity e2 = reg.create();
  Entity e3 = reg.create();
  Entity e4 = reg.create();
  Entity e5 = reg.create();

  // reg.assign<Transform>(e1);
  reg.assign<Transform>(e1);
  reg.assign<Transform>(e1, 1.4f, 3.4f, 5.0f, 6.5f, 2.0f);
  std::cout << reg.getcomponent<Transform>(e1).pos_x << '\n';
  // reg.assign<Transform>(e1, 1, 3, 5, 6, 2);

  // reg.assign<small>(e1, 4, 5);
  // reg.assign<small>(e2, 92, 40);
  // reg.assign<small>(e5, 342, 388);
  // reg.assign<small>(e3, 81, 5);
  // reg.assign<small>(e3, 438279, 312);
  // reg.assign<Transform>(e4);

  // registry.ctest<small>(4);
  // registry.assign<Transform>(8, { 3, 4 }, { 55, 3 }, { 3, 56 });
  // registry.construct<Transform>({ 3, 4 }, { 55, 3 }, { 3, 56 });

  // registry.construct2({ 1, 4 }, { 1, 4 });
  // Transform test{
  //  { 3, 4 }, { 55, 3 }, { 3, 56 }
  //};
}
