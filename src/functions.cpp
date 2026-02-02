#include "functions.h"

namespace Functions
{

  /////// entities ///////

  // size_t seemingly is the largest object size for any platform

  // const bool match = (dense[sparse[entity]] == entity);

  /////// components ///////

  struct Transform {
    sf::Vector2<float> position;
    sf::Vector2<float> rotation;
    sf::Vector2<float> velocity;
  };

  struct vectest {
    std::vector<float> vec;
  };

  struct Sprite {
    sf::Texture texture;
    sf::Sprite  sprite{ texture };
  };

  struct Health {
    int hp;
  };

  struct small {
    int num1;
    int num2;
  };

  // simplest implementation of a sparse set possible
  template <typename Entity, typename Allocator = std::allocator<Entity>>
  class sparse_set {
public:
    virtual void test() {
      std::cout << "Base" << std::endl;
    }

    // check if set exists
    virtual bool check_exists(Entity& entity) {
      // not sure how to best get around entity id 0
      // probably just gonna make entity id start at 1 and 0 in dense array will mean lack of entity
      // i haven't done anything with deletion yet so it's whatever

      // std::cout << dense[sparse[entity]] << '\n';

      // make sure to not index outside out of bounds
      if (sparse.size() <= entity)
        return 0;

      return (dense[sparse[entity]] != 0 && dense[sparse[entity]] == entity);
    }

    // adds entity to set
    void add(Entity entity) {
      // probably bad system lol
      if (sparse.size() <= entity) {
        sparse.resize(entity + 1);
      }

      // dense[sparse[entity]] == entity

      // so this works, but only as long as you're adding onto the end of it i think
      Entity denseindex = dense.size();
      sparse[entity]    = denseindex;
      dense.push_back(entity);

      std::cout << "entity: " << entity << '\n';
      std::cout << "sparse[entity]: " << sparse[entity] << '\n';
      std::cout << "dense[sparse[entity]]: " << dense[sparse[entity]] << '\n';
      std::cout << "sparse size: " << sparse.size() << std::endl;
    }

protected:
    std::vector<Entity, Allocator> dense;
    std::vector<Entity, Allocator> sparse;
  };

  // modified sparse set for storing components
  // polymorphism allows for the use of a base sparse_set pointer to point to a type_storage of any type
  template <typename Type, typename Entity, typename Allocator>
  class type_storage: public sparse_set<Entity, Allocator> {
public:
    void test() override {
      std::cout << "Derived" << std::endl;
    }

    // add entity component data or update component
    template <typename... Args>
    void update(Entity& entity, Args&&... args) {
      // add entity to pool if it doesn't exist
      if (!this->check_exists(entity)) {
        this->add(entity);
        components.push_back(Type{ args... });
        Type& component{ components[this->sparse[entity]] };

        return;
      }
      // if exists, update component with new data
      Type& component{ components[this->sparse[entity]] };
      component = Type{ args... };

      // std::cout << "Exists: " << this->check_exists(entity) << '\n';
    }

private:
    std::vector<Type> components;
  };

  /*  pools map stores component pools and component id
      component pools stores std::shared_ptr to sparse_set base type, and allocates storage for the sparse_set subclass type_storage. i really don't know enough to explain how that works yet

      pool stores:
      id that is a hash of the stored type
      std::shared_ptr to sparse_set base type that was then reallocated as its subclass type_storage

      excerpt from entt docs
      "Pools of components are specialized versions of the sparse set class. Each pool contains all the instances of a single component type and all the entities to which it is assigned."

      pools (map that stores dense sets for components, hashes component type for id, and components themselves somehow)
      component pool
      */

  template <typename Entity, typename Allocator = std::allocator<Entity>>
  struct Registry {
    using id_type = uint32_t; // component id type

    Entity entity_count{ 0 };

    // idk a good way to implement this so i'll just use a basic counter
    Entity createEntity() {
      ++entity_count;
      std::cout << "created entity id: " << entity_count << std::endl;

      return entity_count;
    }

    // keeping this here for now just for ease of access
    // unordered_map<id_type, shared_ptr<base_real>, Entity, std::equal_to<>, typename alloc_traits::template rebind_alloc<std::pair<const id_type, shared_ptr<base_real>>>> pools_real;

    // so while this works, i'm kinda horrified to see how it's laid out in memory. i can only imagine just how janky this setup actually is
    template <typename Type, typename... Args>
    void assign(Entity& entity, Args&&... args) {
      // TODO: add id hash function to prevent adding the same component pool again

      // id_type pool_id=std::hash<Type>{}();
      // std::hash<Type>(4) test;
      // size_t test = std::hash<int>{}(4);

      using cpool_type = type_storage<Type, Entity, Allocator>;

      std::allocator<base_type>  alloc_base;
      std::allocator<cpool_type> alloc_derived;
      // std::shared_ptr<base_type> cpool = std::allocate_shared<base_type>(alloc_derived); // haven't tested but i think this works better right? idk. will test later but for now probably shouldn't fuck wit it
      std::shared_ptr<base_type> cpool = std::allocate_shared<base_type>(alloc_base);

      cpool = std::allocate_shared<cpool_type>(alloc_derived);

      // TODO: hash id type
      id_type id{ 4 };

      // add component pool to pools
      pools.try_emplace(id, cpool);

      // auto&       autoref = static_cast<cpool_type&>(*pools.at(id));
      cpool_type& storage = static_cast<cpool_type&>(*pools.at(id));

      // add component to storage
      storage.update(entity, args...);
    }

    // seemingly not even entt has a method for making this not annoying
    template <typename... Args>
    // void construct2(Args&&...args) { // doesn't work
    void construct2(sf::Vector2<int>, sf::Vector2<int>) { // works
    }

    // so this works fine but is very mildly annoying to me
    // still not sure how to get the cpool type from just the base pointer
    // honestly may just not be possible and i'll have to explicitly pass the type
    template <typename Type>
    void ctest(id_type id) {
      auto& pool          = (*pools.at(id));
      using cpool_type    = type_storage<Type, Entity, Allocator>;
      cpool_type& storage = static_cast<cpool_type&>(*pools.at(id));
      // auto& pool = (*pools.at(id));
    }

private:
    using base_type     = sparse_set<Entity, Allocator>;
    using mainpool_type = std::unordered_map<id_type, std::shared_ptr<base_type>>;
    mainpool_type pools;
  };

  void testing() {
    using Entity = uint32_t;
    const int entityLimit{ 10000 };

    Registry<Entity> registry;
    // test

    Entity entity1 = registry.createEntity();
    Entity entity2 = registry.createEntity();
    Entity entity3 = registry.createEntity();
    Entity entity4 = registry.createEntity();

    registry.assign<small>(entity1, 4, 5);
    registry.assign<small>(entity2, 92, 40);
    registry.assign<small>(entity3, 81, 5);
    registry.assign<small>(entity3, 438279, 312);

    // registry.ctest<small>(4);
    // registry.assign<Transform>(8, { 3, 4 }, { 55, 3 }, { 3, 56 });
    // registry.construct<Transform>({ 3, 4 }, { 55, 3 }, { 3, 56 });
    ;
    // registry.construct2({ 1, 4 }, { 1, 4 });
    // Transform test{
    //  { 3, 4 }, { 55, 3 }, { 3, 56 }
    //};

    // create entity
    // Entity entity1 = registry.createEntity();
  }

  /////// main ///////
  void mainfunc() {
  }

} // namespace Functions
