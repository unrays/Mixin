# Mixin
Compile-time C++ experimental ECS engine with CRTP mixin stacks and zero-overhead system pipelines.

*My first prototype of a CRTP engine with mixins, stacks and pipeline, consolidating my learnings of the last few weeks since I started in TMP, CRTP, SFINAE and everything that adorns the magnificent world of meta-programming!*

I'm just trying out a few things with a new concept I personally explored this week called mixins. It's super cool and allows for very efficient pipelines, which is great for a mini game engine, for example :) I'll definitely be updating the project again soon, so stay tuned!

Don't worry, I KNOW IT'S NOT QUITE THERE YET, but I'm trying to lay the groundwork so I can add my ECS :)

*description here really soon :)*

## Code
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

namespace engine3 {
    struct Empty {
        void update() { std::cout << "[Empty] Updating last...\n"; }
    };

    template<typename Implementation>
    struct ISystem {
        friend Implementation;

    public:
        void info() {
            std::cout << "MOCK INFO SYSTEM\n";
        }

        //juste des methodes utilitaires genre info()
    };

    template<typename Implementation>
    struct IUpdatable {
        friend Implementation;

    public:
        void update() {
            static_cast<Implementation*>(this)->on_update();
        }
    };

    // possiblement faire des ensembles ou packs
    template<typename Next> // next est useless
    struct PositionSystem // pourrait faire quelque chose d'injectable
         : public ISystem<PositionSystem<Next>>,
           public IUpdatable<PositionSystem<Next>>
    {     

    private:
        void on_update() {
            std::cout << "[PositionSystem] Updating...\n";
        }
    };

    template<typename Next>
    struct VelocitySystem
        : ISystem<VelocitySystem<Next>>,
          IUpdatable<VelocitySystem<Next>>
    {
    private:
        void on_update() {
            std::cout << "[VelocitySystem] Updating...\n";
        }
    };

    template<typename Next>
    struct RenderingSystem
         : ISystem<RenderingSystem<Next>>,
           IUpdatable<RenderingSystem<Next>>
    {
    private:
        void on_update() {
            std::cout << "[RenderingSystem] Updating...\n";
        }
    };

    //potentiellement mettre dans Engine
    template<template<typename> class... Layers>
    struct LayeredPipeline
        : private Layers<LayeredPipeline<Layers...>>... {
    public:
        void update_all() {
            (static_cast<Layers<LayeredPipeline>*>(this)->update(), ...);
        }

    private:

    };
    template<template<typename> class... Layers>
    using Pipeline = LayeredPipeline<Layers...>;
}
```

```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

int main() {
    using namespace engine3;

    Pipeline<
        PositionSystem,
        VelocitySystem
    > pipeline;

    PositionSystem<VelocitySystem<Empty>> posys;

    posys.update();

    posys.info();
}
```
