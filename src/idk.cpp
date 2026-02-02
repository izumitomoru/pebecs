extern int s_componentCounter{ 0 };
template <class T>
int getId() {
  static int s_componentId = s_componentCounter;
  return s_componentId;
}

std::size_t create_entity() {
  static std::size_t entities = 0;
  ++entities;
  max_entity = entities;
  return entities;
}

struct ComponentMask {
  void set();
};

struct Scene {
  struct EntityDesc {
    EntityID      id;
    ComponentMask mask;
  };
  std::vector<EntityDesc> entities;

  EntityID NewEntity() {
    entities.push_back({ entities.size(), ComponentMask() });
    return entities.back().id;
  }

  template <typename T>
  void Assign(EntityID id) {
    int componentId = GetId<T>();
    entities[id].mask.set(componentId);
  }
};

