# Mixin
Compile-time C++ experimental ECS engine with CRTP mixin stacks and zero-overhead system pipelines.

*My first prototype of a CRTP engine with mixins, stacks and pipeline, consolidating my learnings of the last few weeks since I started in TMP, CRTP, SFINAE and everything that adorns the magnificent world of meta-programming!*

---

Written on Saturday evening, December 20th

Hey, welcome to my final year personal project for 2025, I hope you're doing well! Today I'm presenting the result of my month of learning CRTP and TMP (and more) in the form of a game engine structure that I'd like to then integrate into my ECS, which was already in preparation. During this project (which spanned a week and a half, during my final exams), I experimented and iterated on numerous techniques and idioms related to metaprogramming and static polymorphism. In particular, I learned to use and understand the concept of 'friend' in C++, which is extremely useful for separating and segregating responsibilities and contracts without any coupling between classes. Furthermore, I've significantly improved my skills with static polymorphism to where I am today. I'm quite confident with this concept and I'm having more and more fun with it. I particularly love the mix of freedom and performance it offers; it's risky, yes, but when used correctly, it's extremely powerful. :) 

Furthermore, during this project, I experimented with the concept of Mixins and compile-time composition, which notably allowed me to 'solidify' my architecture by clearly separating business and logical contracts from the objects that execute them (we love SOLID here). Additionally, Mixin Stacks (or clusters, I can't find the correct definition) enabled me to create a nesting system that allows me to build independent and very flexible pipelines, all of this, again, 100% compile-time ;) Finally, I really experimented a lot with the power offered by CRTP and TMP. In fact, I've always wanted to create this kind of architecture where I decide the rules; however, with OOP and normal programming, I've always felt like I had 'magical' barriers that I absolutely couldn't overcome (virtual polymorphism). Now that I'm armed with these concepts, I can finally do what I want and imagine the craziest things possible, all while improving my coding and architectural thinking skills!

I think you're starting to understand that I'm in love with the concepts that touch on meta-programming. In fact, since I discovered this a month ago, I only think in terms of compile-time concepts instead of runtime. It's super strange, however, you also have to find the right balance, but I'm starting to understand the rules of the game! So, as I was saying, I worked extensively on the API and Library side of my project. I personally value cleanliness and simplicity much more than complex and unreadable code, and that's why I coded this system with the aim of making it usable by someone at the level I was at when I started the program (3 weeks ago, I think). We just hide enough abstraction and the underlying 'magic' system and leave an ultra-minimal structure for the programmer so that they can focus on their real task: coding and doing what they are passionate about (not spending 8 hours trying to understand code that doesn't interest them at all).

For example, you only need to look at the System class, which is a mock of a system. In the 'full' version you see below, this system is a bit more complex because it incorporates the IUpdateLinkable contract, which specifies that it must be chain-updatable with the templated Next parameter. However, if you look further down in the original code I wrote for the hook system, you'll see that the boilerplate is very minimal and that the system only exposes the functions it needs to expose—in our case, update(). In fact, the goal here was to minimize coupling and increase the flexibility of the classes. For example, I worked hard to ensure that the system hooks are private, which prevents them from being called during runtime and completely breaking the system (very bad if that happens). Furthermore, I separated my system into Mixins and crtp layers in order to reduce the size of the monolith that a system class often is; here, in my architecture, it is just simple enough to be used by a beginner, and I am proud of it.

Well, I think I've said enough for today. I wish you a wonderful rest of the day and a happy holiday season, a magnificent end to the year ;) Until next time!

---

```console
Creating new system...
Creating new system...
Creating new system...
No 'onPreUpdate' using default.
Updating System...
No 'onPostUpdate' using default.
Calling next
No 'onPreUpdate' using default.
Updating System...
No 'onPostUpdate' using default.
Calling next
No 'onPreUpdate' using default.
Updating System...
No 'onPostUpdate' using default.
Calling next
Updating Empty system...
Destroying system...
Destroying system...
Destroying system...
```

## Code
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

int main() {
    using namespace engine8;

    using NetworkPipeline = SystemA<SystemB<SystemB<SystemA<Empty>>>>;
    using GraphicPipeline = SystemA<SystemB<SystemA<Empty>>>;
    using PhysicsPipeline = SystemA<SystemA<SystemB<Empty>>>;
    using AudioPipeline = SystemB<SystemB<SystemtA<Empty>>>;
    using UIPipeline = SystemB<SystemA<Empty>>;

    Pipeline<
        SystemA<SystemB<SystemB<SystemA<Empty>>>>
    > networkPipeline;

    Pipeline<
        SystemA<SystemB<SystemtA<Empty>>>
    > graphicPipeline;

    PipelineExecutor<
        Pipeline<SystemA<SystemB<SystemA<Empty>>>>,
        Pipeline<SystemB<SystemA<Empty>>>,
        Pipeline<SystemB<SystemB<Empty>>>
    > pipelineDispatcher;

    Engine<
        PipelineExecutor<
            Pipeline<SystemA<SystemB<SystemA<Empty>>>>,
            Pipeline<SystemB<SystemA<Empty>>>,
            Pipeline<SystemB<SystemB<Empty>>>
        >
    > engine;

    //engine.update_all();

    System<System<System<Empty>>> sys;
    sys.update();
}
```

```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

namespace engine8 {
    template<typename Hooked>
    struct IEventHookable {
    protected:
        IEventHookable() {
            if constexpr (requires(Hooked h) { h.onCreated(); })
                static_cast<Hooked*>(this)->onCreated();
            else onCreatedDefault();
        }
    
        ~IEventHookable() {
            if constexpr (requires(Hooked h) { h.onDestroyed(); })
                static_cast<Hooked*>(this)->onDestroyed();
            else onDestroyedDefault();
        }
    
    protected:
        void invokePreUpdate() {
            if constexpr (requires(Hooked h) { h.onPreUpdate(); })
                static_cast<Hooked*>(this)->onPreUpdate();
            else onPreUpdateDefault();
        }
    
        void invokeUpdate() {
            invokePreUpdate();
            if constexpr (requires(Hooked h) { h.onUpdate(); })
                static_cast<Hooked*>(this)->onUpdate();
            else onUpdateDefault();
            invokePostUpdate();
        }
    
        void invokePostUpdate() {
            if constexpr (requires(Hooked h) { h.onPostUpdate(); })
                static_cast<Hooked*>(this)->onPostUpdate();
            else onPostUpdateDefault();
        }
    
    protected:
        void onCreatedDefault() { std::cout << "No 'onCreated' using default.\n"; }
        void onDestroyedDefault() { std::cout << "No 'onDestroyed' using default.\n"; }
    
        void onUpdateDefault() { std::cout << "No 'onUpdate' using default.\n"; }
        void onPreUpdateDefault() { std::cout << "No 'onPreUpdate' using default.\n"; }
        void onPostUpdateDefault() { std::cout << "No 'onPostUpdate' using default.\n"; }
    };
    
    template<typename Derived>
    struct IUpdatable {
        void update()
            requires std::is_base_of_v<IEventHookable<Derived>, Derived>
        { static_cast<Derived*>(this)->invokeUpdate(); }
    };
    
    struct Empty {
        void update() {
            std::cout << "Updating Empty system...\n";
        }
    };
    
    template<typename Derived, typename Next>
    struct IUpdateLinkable {
    public:
        void update()
            requires requires(Next& n) { n.update(); }
            && std::is_base_of_v<IEventHookable<Derived>, Derived>
        { 
            static_cast<Derived*>(this)->invokeUpdate();
            std::cout << "Calling next\n";
            next_.update();
        }
    
    private:
        Next next_;
    
    };
    
    #define SYSTEM_HOOKS(Derived, Next)           \
        IUpdateLinkable<Derived, Next>,           \
        IEventHookable<Derived>                  
    
    #define SYSTEM_FRIENDS(Derived, Next)         \
        friend IEventHookable<Derived>;           \
        friend IUpdateLinkable<Derived, Next>;
    
    template<typename Next>
    struct System : SYSTEM_HOOKS  (System<Next>, Next) {
    public:         SYSTEM_FRIENDS(System<Next>, Next)
    private:
        void onCreated() {
            std::cout << "Creating new system...\n";
        }
    
        void onUpdate() {
            std::cout << "Updating System...\n";
        }
    
        void onDestroyed() {
            std::cout << "Destroying system...\n";
        }
    };
    
    template<typename Layers>
    struct Pipeline : private Layers {
        template<typename, typename...>
        friend struct PipelineExecutor;
        using type = Layers;
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
    
    template<typename... Components>
    struct Engine : Components... {
        //genre sfinae {if un des variadic est un pipeline, iterer}
    
        //normalement, je penses pas qu'il y aura tant de code que ca ici
        //il devrait hériter des components qui sont des specialisations
    
        //a la limite, faire un system qui passe registry sur toutes les étages
    };
}
```

# Latest hook system re-built from the ground up since my first and catastrophic try

##### *I'm seriously very proud of this result; I really worked hard to create a very simple and intuitive API! The beauty of it is that when you instantiate a new System, the only method available to execute for the object is .update(), only that one, nothing else. I think that's excellent, I'm very happy :)*
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

template<typename Hooked>
struct IEventHookable {
protected:
    IEventHookable() {
        if constexpr (requires(Hooked h) { h.onCreated(); })
            static_cast<Hooked*>(this)->onCreated();
        else onCreatedDefault();
    }

    ~IEventHookable() {
        if constexpr (requires(Hooked h) { h.onDestroyed(); })
            static_cast<Hooked*>(this)->onDestroyed();
        else onDestroyedDefault();
    }

protected:
    void invokePreUpdate() {
        if constexpr (requires(Hooked h) { h.onPreUpdate(); })
            static_cast<Hooked*>(this)->onPreUpdate();
        else onPreUpdateDefault();
    }

    void invokeUpdate() {
        invokePreUpdate();
        if constexpr (requires(Hooked h) { h.onUpdate(); })
            static_cast<Hooked*>(this)->onUpdate();
        else onUpdateDefault();
        invokePostUpdate();
    }

    void invokePostUpdate() {
        if constexpr (requires(Hooked h) { h.onPostUpdate(); })
            static_cast<Hooked*>(this)->onPostUpdate();
        else onPostUpdateDefault();
    }

protected:
    void onCreatedDefault() { std::cout << "No 'onCreated' using default.\n"; }
    void onDestroyedDefault() { std::cout << "No 'onDestroyed' using default.\n"; }

    void onUpdateDefault() { std::cout << "No 'onUpdate' using default.\n"; }
    void onPreUpdateDefault() { std::cout << "No 'onPreUpdate' using default.\n"; }
    void onPostUpdateDefault() { std::cout << "No 'onPostUpdate' using default.\n"; }
};

template<typename Derived>
struct IUpdatable {
    void update()
        requires std::is_base_of_v<IEventHookable<Derived>, Derived>
    { static_cast<Derived*>(this)->invokeUpdate(); }
};

struct System : public IEventHookable<System>, public IUpdatable<System> {
public:         friend IEventHookable<System>; friend IUpdatable<System>;
private:
    void onCreated() {
        std::cout << "Creating new system...\n";
    }

    void onUpdate() {
        std::cout << "Updating System...\n";
    }

    void onDestroyed() {
        std::cout << "Destroying system...\n";
    }
};

int main() {
    System sys;
    sys.update();

    //sys. -> [ update() ]
}
```

```console
Creating new system...
No 'onPreUpdate' using default.
Updating System...
No 'onPostUpdate' using default.
Destroying system...
```

# Hook system I WAS currently working on :)
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
