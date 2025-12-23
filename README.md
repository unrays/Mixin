# Mixin
Compile-time C++ experimental ECS engine with CRTP mixin stacks and zero-overhead system pipelines.

*My first prototype of a CRTP engine with mixins, stacks and pipeline, consolidating my learnings of the last few weeks since I started in TMP, CRTP, SFINAE and everything that adorns the magnificent world of meta-programming!*

---

Written on Saturday evening, December 20th

Hey, welcome to my final year personal project for 2025, I hope you're doing well! Today I'm presenting the result of my month of learning CRTP and TMP (and more) in the form of a game engine structure that I'd like to then integrate into my ECS, which was already in preparation. During this project (which spanned a week and a half, during my final exams), I experimented and iterated on numerous techniques and idioms related to metaprogramming and static polymorphism. In particular, I learned to use and understand the concept of 'friend' in C++, which is extremely useful for separating and segregating responsibilities and contracts without any coupling between classes. Furthermore, I've significantly improved my skills with static polymorphism to where I am today. I'm quite confident with this concept and I'm having more and more fun with it. I particularly love the mix of freedom and performance it offers; it's risky, yes, but when used correctly, it's extremely powerful. :) 

Furthermore, during this project, I experimented with the concept of Mixins and compile-time composition, which notably allowed me to 'solidify' my architecture by clearly separating business and logical contracts from the objects that execute them (we love SOLID here). Additionally, Mixin Stacks (or clusters, I can't find the correct definition) enabled me to create a nesting system that allows me to build independent and very flexible pipelines, all of this, again, 100% compile-time ;) Finally, I really experimented a lot with the power offered by CRTP and TMP. In fact, I've always wanted to create this kind of architecture where I decide the rules; however, with OOP and normal programming, I've always felt like I had 'magical' barriers that I absolutely couldn't overcome (virtual polymorphism). Now that I'm armed with these concepts, I can finally do what I want and imagine the craziest things possible, all while improving my coding and architectural thinking skills!

I think you're starting to understand that I'm in love with the concepts that touch on meta-programming. In fact, since I discovered this a month ago, I only think in terms of compile-time concepts instead of runtime. It's super strange, however, you also have to find the right balance, but I'm starting to understand the rules of the game! So, as I was saying, I worked extensively on the API and Library side of my project. I personally value cleanliness and simplicity much more than complex and unreadable code, and that's why I coded this system with the aim of making it usable by someone at the level I was at when I started the program (3 weeks ago, I think). We just hide enough abstraction and the underlying 'magic' system and leave an ultra-minimal structure for the programmer so that they can focus on their real task: coding and doing what they are passionate about (not spending 8 hours trying to understand code that doesn't interest them at all).

For example, you only need to look at the System class, which is a mock of a system. In the 'full' version you see below, this system is a bit more complex because it incorporates the IUpdateLinkable contract, which specifies that it must be chain-updatable with the templated Next parameter. However, if you look further down in the original code I wrote for the hook system, you'll see that the boilerplate is very minimal and that the system only exposes the functions it needs to expose, in our case, update(). In fact, the goal here was to minimize coupling and increase the flexibility of the classes. For example, I worked hard to ensure that the system hooks are private, which prevents them from being called during runtime and completely breaking the system (very bad if that happens). Furthermore, I separated my system into Mixins and crtp layers in order to reduce the size of the monolith that a system class often is; here, in my architecture, it is just simple enough to be used by a beginner, and I am proud of it.

I've also started switching from SFINAE to Concept. I started with SFINAE because I wanted to learn the hard way, and I think it worked well because I find Concept and Requires ultimately much easier than SFINAE and type traits and all that ;) I don't know if I'll abandon SFINAE right away because it's a very important asset, especially if I ever have to work in an environment that hasn't yet switched to the new standards, so I'll be careful! Well, nothing's stopping me from doing both, you know. I'm going to try to become a wizard in both spheres; that way, it's even more magical :)

Well, I think I've said enough for today. I wish you a wonderful rest of the day and a happy holiday season, a magnificent end to the year ;) Until next time!

---

```console
Creating new System...
Creating new Linked System...
Creating new Linked System...
Creating new Linked System...
Creating new Pipeline...


No 'onPreUpdate' using default.
Updating Linked System...
No 'onPostUpdate' using default.
Calling next
No 'onPreUpdate' using default.
Updating Linked System...
No 'onPostUpdate' using default.
Calling next
No 'onPreUpdate' using default.
Updating Linked System...
No 'onPostUpdate' using default.
Calling next
No 'onPreUpdate' using default.
Updating System...
Post updating the last system of the stack...


Destroying Pipeline...
Destroying Linked System...
Destroying Linked System...
Destroying Linked System...
Destroying System...
```

## Code
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

int main() {
    using namespace engine;

    using NetworkPipeline = SystemA<SystemB<SystemB<SystemA<Empty>>>>;
    using GraphicPipeline = SystemA<SystemB<SystemA<Empty>>>;
    using PhysicsPipeline = SystemA<SystemA<SystemB<Empty>>>;
    using AudioPipeline = SystemB<SystemB<SystemA<Empty>>>;
    using UIPipeline = SystemB<SystemA<Empty>>;

    Pipeline<
        SystemA<SystemB<SystemB<SystemA<Empty>>>>
    > networkPipeline;

    Pipeline<
        SystemA<SystemB<SystemA<Empty>>>
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

    PipelineExecutor<
        Pipeline<LinkedSystem<LinkedSystem<LinkedSystem<System>>>>
    > pipeline_exec;

    pipeline_exec.update_all();
}
```

```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

namespace engine {
    template<typename Hooked>
    struct EventHookable {
    protected:
        EventHookable() {
            if constexpr (requires(Hooked h) { h.onCreated(); })
                static_cast<Hooked*>(this)->onCreated();
            else onCreatedDefault();

            //std::cout << "System @" << this << '\n';

        }

        ~EventHookable() {
            if constexpr (requires(Hooked h) { h.onDestroyed(); })
                static_cast<Hooked*>(this)->onDestroyed();
            else onDestroyedDefault();

            //std::cout << "System @" << this << '\n';

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

        //possiblement réfléchir sur un hook onNext();
    };

    template<typename Derived>
    struct Updatable {
        void update()
            requires std::is_base_of_v<EventHookable<Derived>, Derived>
        {
            static_cast<Derived*>(this)->invokeUpdate();
        }
    };

    struct EmptyLink {
        void update() {
            std::cout << "Updating Empty system...\n";
        }
    };

    template<typename Derived, typename Next>
    struct UpdateLinkable {
    public:
        void update()
            requires requires(Next& n) { n.update(); }
            && std::is_base_of_v<EventHookable<Derived>, Derived>
        {
            static_cast<Derived*>(this)->invokeUpdate();
            std::cout << "Calling next\n";
            next_.update();
        }

    private:
        Next next_;

    };

#pragma region [System] Prototype
    struct System : public EventHookable<System>, public Updatable<System> {
    public:         friend EventHookable<System>; friend Updatable<System>;
    private:
        void onCreated() {
            std::cout << "Creating new System...\n";
        }

        void onUpdate() {
            std::cout << "Updating System...\n";
        }

        void onPostUpdate() {
            std::cout << "Post updating the last system of the stack...\n";
        }

        void onDestroyed() {
            std::cout << "Destroying System...\n";
        }
    };
#pragma endregion

#pragma region [Linked System] Prototype
#define SYSTEM_HOOKS(Derived, Next)         \
        UpdateLinkable<Derived, Next>,          \
        EventHookable<Derived>                  

#define SYSTEM_FRIENDS(Derived, Next)       \
        friend EventHookable<Derived>;          \
        friend UpdateLinkable<Derived, Next>;

    template<typename Next>
    struct LinkedSystem : SYSTEM_HOOKS  (LinkedSystem<Next>, Next) {
    public:               SYSTEM_FRIENDS(LinkedSystem<Next>, Next)
    private:
        void onCreated() {
            std::cout << "Creating new Linked System...\n";
        }

        void onUpdate() {
            std::cout << "Updating Linked System...\n";
        }

        void onDestroyed() {
            std::cout << "Destroying Linked System...\n";
        }
    };
#pragma endregion

    template<typename Layers>
    struct Pipeline : private Layers {
        template<typename, typename...>
        friend struct PipelineExecutor;
        using type = Layers;

    public:
        Pipeline() {
            std::cout << "Creating new Pipeline...\n";
        }

        ~Pipeline() {
            std::cout << "Destroying Pipeline...\n";
        }
    };

    template<typename First, typename... Rest>
    struct PipelineExecutor {
    public:
        PipelineExecutor() : pipelines_() {}

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

    /*=========================================*/

    template<typename Derived>
    struct Singleton {
    public:
        static Derived& instance() {
            static Derived instance(Token{});
            return instance;
        }

    protected:
        struct Token {};

        Singleton(Token) = default;

    private:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton&&) = delete;
    };

    struct World : Singleton<World> {
        template<typename T>
        friend struct WorldProxy;

    public:
        struct Configuration {
            using QueryReturnType = int;
        };

    public:
        void invoke() const {
            std::cout << "Ivoking World\n";
        }

    private:
        void query() {
            std::cout << "World -> query...\n";
        }

        void emplace() {
            std::cout << "World -> emplace...\n";
        }

        void update() {
            std::cout << "pas certain que c'est une fonc de world...\n";
        }

    private:
        // ECS <- CRTP <- SYSTEM CALL
        // crtp qui garde la reference vers le ecs
        // system qui call son crtp dans update
        // j'imagine qu'il pourrait genre le query
        // genre crtp::query([](){ lambda }) qui return ecs.query(->&lambda) view
    };

    template<typename Derived>
    struct WorldProxy {
    public:
        void test() {
            std::cout << "test\n";
        }

        World& forward() {
            return World::instance();
        }

        void query_world() { //faire un using type dans ecs pour determiner le tuype de retour
            World::getInstance().query();
        }



    private:


    };

    struct MockSystem : private WorldProxy<MockSystem> {
    public:
        void update() {
            //this->forward().invoke();

            this->query_world();
        }

    private:


    };
}
```

# Latest progress on the ECS architecture

##### *It's really really cool, custom CRTP Meyers Singleton + Proxy for secure communication with systems*
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

template<typename Derived>
struct Singleton {
public:
    static Derived& instance() {
        static Derived instance(Token{});
        return instance;
    }

protected:
    struct Token {};

    explicit Singleton(Token) {}

private:
    Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};

struct World : Singleton<World>, EventHookable<World> {
public:
    friend struct WorldProxy;
    explicit World(typename Singleton::Token t)
        : Singleton<World>(t) {}
    friend EventHookable<World>;

private:
    void onCreated() {
        std::cout << "Creating World...\n";
        // mettre l'initialisation des sparse set typés
    }

    void onDestroyed() {
        std::cout << "Destroying World...\n";
    }

private:
    void query() {
        std::cout << "query() not implemented yet\n";
    }

    void emplace() {
        std::cout << "emplace() not implemented yet\n";
    }

    void get() {
        std::cout << "get() not implemented yet\n";
    }

    void remove() {
        std::cout << "remove() not implemented yet\n";
    }
};

struct WorldProxy {
protected:
    void query() {
        World::instance().query();
    }

    void emplace() {
        World::instance().emplace();
    }

    void get() {
        World::instance().get();
    }

    void remove() {
        World::instance().remove();
    }

private:

};

struct MockSystem : private WorldProxy, 
    public EventHookable<MockSystem>, public Updatable<MockSystem> {
    friend EventHookable<MockSystem>; friend Updatable<MockSystem>;

private:
    void onCreated() {
        std::cout << "Creating MockSystem\n";
    }

    void onUpdate() {
        this->query();
    }

};
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
