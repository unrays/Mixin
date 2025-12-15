# Mixin
Compile-time C++ experimental ECS engine with CRTP mixin stacks and zero-overhead system pipelines.

*My first prototype of a CRTP engine with mixins, stacks and pipeline, consolidating my learnings of the last few weeks since I started in TMP, CRTP, SFINAE and everything that adorns the magnificent world of meta-programming!*

I'm just trying out a few things with a new concept I personally explored this week called mixins. It's super cool and allows for very efficient pipelines, which is great for a mini game engine, for example :) I'll definitely be updating the project again soon, so stay tuned!

Don't worry, I KNOW IT'S NOT QUITE THERE YET, but I'm trying to lay the groundwork so I can add my ECS :)

*description here really soon :)*

```console
[ComponentA] updating...
[ComponentB] updating...
[ComponentA] updating...
[Empty] Updating last...

[ComponentB] updating...
[ComponentA] updating...
[Empty] Updating last...

[ComponentB] updating...
[ComponentB] updating...
[Empty] Updating last...
```

## Code
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

int main() {
    using namespace engine8;

    using NetworkPipeline = ComponentA<ComponentB<ComponentB<ComponentA<Empty>>>>;
    using GraphicPipeline = ComponentA<ComponentB<ComponentA<Empty>>>;
    using PhysicsPipeline = ComponentA<ComponentA<ComponentB<Empty>>>;
    using AudioPipeline = ComponentB<ComponentB<ComponentA<Empty>>>;
    using UIPipeline = ComponentB<ComponentA<Empty>>;

    Pipeline<
        ComponentA<ComponentB<ComponentB<ComponentA<Empty>>>>
    > networkPipeline;

    Pipeline<
        ComponentA<ComponentB<ComponentA<Empty>>>
    > graphicPipeline;

    PipelineExecutor<
        Pipeline<ComponentA<ComponentB<ComponentA<Empty>>>>,
        Pipeline<ComponentB<ComponentA<Empty>>>,
        Pipeline<ComponentB<ComponentB<Empty>>>
    > pipelineDispatcher;

    Engine<
        PipelineExecutor<
            Pipeline<ComponentA<ComponentB<ComponentA<Empty>>>>,
            Pipeline<ComponentB<ComponentA<Empty>>>,
            Pipeline<ComponentB<ComponentB<Empty>>>
        >
    > engine;

    engine.update_all();
}
```

```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

namespace engine8 {
    struct Empty {
        void update() { std::cout << "[Empty] Updating last...\n"; }
    };

    template<typename Implementation>
    struct IComponent {
        friend Implementation;

    public:
        void update() {
            static_cast<Implementation*>(this)->on_update();
        }
    };

    template<typename Next = Empty>
    struct ComponentA : IComponent<ComponentA<Next>> {
        friend IComponent<ComponentA<Next>>;
    protected:
        void on_update() {
            std::cout << "[ComponentA] updating...\n";
            next_.update();
        }

    private:
        Next next_;
    };

    template<typename Next = Empty>
    struct ComponentB : IComponent<ComponentB<Next>> {
        friend IComponent<ComponentB<Next>>;

    protected:
        void on_update() {
            std::cout << "[ComponentB] updating...\n";
            next_.update();
        }

    private:
        Next next_;
    };

    template<typename First, typename... Rest>
    struct PipelineExecutor {
    public:
        PipelineExecutor() : pipelines_(First{}, Rest{}...) {}

    public:
        void update_all() noexcept {
            std::apply([](auto&&... args) {
                ((args.update()), ...);
            }, pipelines_);
        }

    private:
        std::tuple<First, Rest...> pipelines_;
    };


    template<typename Layers>
    struct Pipeline : private Layers {
        template<typename, typename...>
        friend struct PipelineExecutor;
        using type = Layers;
    };

    template<typename... Components>
    struct Engine : Components... {
        //genre sfinae {if un des variadic est un pipeline, iterer}

        //normalement, je penses pas qu'il y aura tant de code que ca ici
        //il devrait hériter des components qui sont des specialisations
    };
}
```
