# Mixin
Compile-time C++ experimental ECS engine with CRTP mixin stacks and zero-overhead system pipelines.

*My first prototype of a CRTP engine with mixins, stacks and pipeline, consolidating my learnings of the last few weeks since I started in TMP, CRTP, SFINAE and everything that adorns the magnificent world of meta-programming!*

I'm just trying out a few things with a new concept I personally explored this week called mixins. It's super cool and allows for very efficient pipelines, which is great for a mini game engine, for example :) I'll definitely be updating the project again soon, so stay tuned!

Don't worry, I KNOW IT'S NOT QUITE THERE YET, but I'm trying to lay the groundwork so I can add my ECS :)

*description here really soon :)*

It's looking pretty rough in it's current form, I'm planning to work on it as soon as my exam week is finished.

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

# Latest hook system re-built from the ground up since my first and catastrophic try
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

template<typename Hooked>
struct IEventHookable {
public:
    IEventHookable() { //static pour faire un seul check
        if constexpr (requires(Hooked h) { h.onCreated(); })
            static_cast<Hooked*>(this)->onCreated();
        else onCreatedDefault();
    }

    ~IEventHookable() {
        if constexpr (requires(Hooked h) { h.onDestroyed(); })
            static_cast<Hooked*>(this)->onDestroyed();
        else onDestroyedDefault();
    }

public:
    void onCreatedDefault() { std::cout << "No 'onCreated' using default.\n"; }
    void onDestroyedDefault() { std::cout << "No 'onDestroyed' using default.\n"; }
};

struct System : private IEventHookable<System> {
public:          friend IEventHookable<System>;
private:
    void onCreated() {
        std::cout << "Creating new system...\n";
    }

    void onDestroyed() {
        std::cout << "Destroying system...\n";
    }
};
```

# Hook system I'm currently working on :)
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

namespace hooks {
    namespace Traits {
        template<typename T, typename Enable = void>
        struct has_on_created : std::false_type {};

        template<typename T>
        struct has_on_created<
            T,
            typename std::void_t<
            decltype(std::declval<T>().on_created())>
        > : std::true_type {
        };

        /*********************************************/

        template<typename T, typename Enable = void>
        struct has_on_destroyed : std::false_type {};

        template<typename T>
        struct has_on_destroyed<
            T,
            typename std::void_t<
            decltype(std::declval<T>().on_destroyed())>
        > : std::true_type {
        };

        /*********************************************/

        template<typename T, typename Enable = void>
        struct has_on_update : std::false_type {};

        template<typename T>
        struct has_on_update<
            T,
            typename std::void_t<
            decltype(std::declval<T>().on_update())>
        > : std::true_type {
        };
    }
    

    template<typename Derived>
    struct Base {
    public:
        Base(std::true_type) {

        }

        Base(std::false_type) {

        }

        Base() {
            std::cout << "test";
        }






    public:
        void foo(int) {
            std::cout << "Int specialization\n";
        }

        void foo(float) {
            std::cout << "Float specialization\n";
        }

        template<typename T>
        auto foo(T) ->
            std::enable_if_t<std::is_same_v<T, bool>, void> {
            std::cout << "Boolean specialization\n";
        }

    private:

    };

    /*template<typename Derived>
    struct Base < Derived, std::enable_if_t < std::is_member_function_pointer_v<decltype(&Derived::onCreated) >>> {
    public:
        void onCreated() override {
            std::cout << "Created specialized base!\n";
            static_cast<Derived*>(this)->onCreated();
        }

        ~Base() {
            std::cout << "Destroyed specialized base!\n";
        }
    };*/

    template<typename Derived>
    struct testBase {
    public:
        void forward_constructor(std::true_type) {
            std::cout << "Function 'on_created' detected and being used.\n";
            static_cast<Derived*>(this)->on_created();
        }

        void forward_constructor(std::false_type) {
            std::cout << "Function 'on_created' not detected, using default...\n";
        }

        testBase() {
            forward_constructor(Traits::has_on_created<Derived>{});
        }

    public:
        void forward_destructor(std::true_type) {
            std::cout << "Function 'on_destroyed' detected and being used.\n";
            static_cast<Derived*>(this)->on_destroyed();
        }

        void forward_destructor(std::false_type) {
            std::cout << "Function 'on_destroyed' not detected, using default...\n";
        }

        ~testBase() {
            forward_destructor(Traits::has_on_destroyed<Derived>{});
        }

    public:
        void forward_update(std::true_type) {
            std::cout << "Function 'on_update' detected and being used.\n";
            static_cast<Derived*>(this)->on_update();
        }

        void forward_update(std::false_type) {
            std::cout << "Function 'on_update' not detected, using default...\n";
        }

        void update() {
            forward_update(Traits::has_on_update<Derived>{});
        }


    private:


    };

    /*on_created()
    on_init()
    on_enable()
    on_disable()
    update()
    fixed_update()
    late_update()
    on_destroy()
    */

    struct testDerived : testBase<testDerived> {
    public:
        void on_created() {
            std::cout << "Creating testDerived...\n";
        }

        void on_destroyed() {
            std::cout << "Destroying testDerived...\n";
        }

        void on_update() {
            std::cout << "Updating testDerived...\n";
        }


    private:


    };

    struct Derived : Base<Derived> {
    public:
        void onCreated() {
            std::cout << "onCreated in Derived\n";
        }

    private:


    };
}
```
