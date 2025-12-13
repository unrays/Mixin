# Mixin
Compile-time C++ experimental ECS engine with CRTP mixin stacks and zero-overhead system pipelines.

*My first prototype of a CRTP engine with mixins, stacks and pipeline, consolidating my learnings of the last few weeks since I started in TMP, CRTP, SFINAE and everything that adorns the magnificent world of meta-programming!*

I'm just trying out a few things with a new concept I personally explored this week called mixins. It's super cool and allows for very efficient pipelines, which is great for a mini game engine, for example :) I'll definitely be updating the project again soon, so stay tuned!

*description here really soon :)*

## Code
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

namespace ecs {
    struct Empty {
        void update() noexcept {}
    };

    template<typename DerivedSystem>
    struct ISystem {
    public:
        void update() noexcept {
            static_cast<DerivedSystem*>(this)->forward_update()();
        }
    };

    template<typename Next>
    struct PositionSystem : ISystem<PositionSystem<Next>> {
    public:
        friend struct ISystem<PositionSystem<Next>>;

    protected:
        void updatePositions() noexcept {
            std::cout << "[PositionSystem] Updating...\n";

        }

    private:
        auto forward_update() noexcept {
            return [this]() { return updatePositions(); };
        }
    };

    template<typename Derived>
    struct CoreInvokerMixin {
        auto invoke_position_update() noexcept {
            static_cast<Derived*>(this)->update();
            // pour l'instant, il apelle tous les update
            //du système, changer prochainement :)
        }
    };

    template<template<typename> class... SystemLayers>
    struct Engine : CoreInvokerMixin<Engine<SystemLayers...>>,
                    SystemLayers<Engine<SystemLayers...>>... {};


    //pas vraiment engine, plus flux d'exec pour le ecs
    //essentiellement un pipeline de systems dynamiquement static :)

    //using PhysicsStack =
    // PositionSystem<VelocitySystem<MovementSystem<EmptySystem>>>;

    /* faire un variadic de stack genre comme des catégories */
}
```

```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

int main() {
  using namespace ecs;

  Engine<PositionSystem> engine;
  engine.invoke_position_update();
}
```
