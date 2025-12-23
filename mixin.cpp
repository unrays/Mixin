#include <functional>
#include <iostream>
#include <concepts>
#include <vector>
#include <print>

namespace v1 {
    struct SizedObject {
    public:
        std::size_t size() const noexcept {
            return size_;
        }

    private:
        std::size_t size_ = 10;
    };

    struct Object {
    public:
        template<typename T>
            requires HasSize<T>
        void extractSize() noexcept {

        }

        template<typename T>
            requires
        std::integral<T>
            void doSomething() {
            std::println("{} value is an integral!", typeid(T).name());
        }

    private:


    };

    template<typename T>
    concept HasSize = requires(T t) {
        t.size();
    };

    template<typename T>
    concept HasIntegralSize = requires(T t) {
        { t.size() } -> std::integral;
    };
}

namespace v2 {
    template<typename Derived>
    struct HookInspector {
        template<typename T>
        static constexpr bool hasOnCreated = requires(T t) { t.onCreated(); };

        void testt() {
            if constexpr (hasOnCreated<Derived>)
                std::cout << "present\n";
            else std::cout << "not present\n";
        }

    };



    //template<typename T>
    //concept HasOnCreated = requires(T t)
    //{ t.onCreated(); };




    template<typename Hooked>
    struct IEventHookable {
    public:
        IEventHookable() { //static pour faire un seul check
            if constexpr (requires(Hooked h) { h.onCreated(); })
                static_cast<Hooked*>(this)->onCreated();
            else std::cout << "No 'onCreated' using default.\n";
        }

        ~IEventHookable() {
            if constexpr (requires(Hooked h) { h.onDestroyed(); })
                static_cast<Hooked*>(this)->onDestroyed();
            else std::cout << "No 'onDestroyed' using default.\n";
        }

    private:


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

    private:
        int secret_ = 10;

    };
}

namespace v3 {
    template<typename Derived>
    struct HookInspector {
        template<typename T>
        static constexpr bool hasOnCreated = requires(T t) { t.onCreated(); };

        void testt() {
            if constexpr (hasOnCreated<Derived>)
                std::cout << "present\n";
            else std::cout << "not present\n";
        }

    };

    //template<typename T>
    //concept HasOnCreated = requires(T t)
    //{ t.onCreated(); };


    //template<typename Implementation>
    //struct ISystem : private IEventHookable<Implementation> {
    //public:           friend IEventHookable<Implementation>;
    //};


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
}

namespace funcx {
    struct Object {
    public:
        void foo() {
            std::cout << "foo()\n";
        }

        void bar() {
            std::cout << "bar()\n";
        }

        void withArgs(int a, int b) {
            std::cout << "withArgs(" << a << ", " << b << ")\n";
        }

        void test() {
            std::cout << "test()\n";
        }

    private:


    };

    struct Callable {
    public:
        template<typename Ret, typename Obj, typename... Args>
        Ret call(Obj& obj, Ret(Obj::* func)(Args...), Args&&... args) {
            return (obj.*func)(std::forward<Args>(args)...);
        }
    };
}

namespace v4 {
    struct Object {
        friend struct Invoker;
    public:
        void public_test(int a) {
            std::cout << "Public fnc: " << a << "\n";
        }

    private:
        void private_test(int a) {
            std::cout << "Private fnc: " << a << "\n";
        }
    };

    struct Invoker {
        template<typename Ret, typename Obj, typename... Args>
        static Ret invoke(Obj& obj, Ret (Obj::*func)(Args...), Args&&... args) {
            return (obj.*func)(std::forward<Args>(args)...);
        }

        //IEventHookable() { //static pour faire un seul check
        //    if constexpr (requires(Hooked h) { h.onCreated(); })
        //        static_cast<Hooked*>(this)->onCreated();
        //    else onCreatedDefault();
        //}

      /*  template<typename Ret, typename Obj, typename... Args>
        static constexpr bool HasFunc = requires(
            Obj & obj, Ret(Obj:: * func)(Args...), Args&&... args
        ) {
            (obj.*func)(std::forward<Args>(args)...)
        };*/

        
        void test() {
            obj.private_test(10);
        }

    private:
        Object obj;
    };



    template<typename Receiver>
    struct Invoker2 : Receiver {
    public:


    private:
        Receiver r_;

    };

    //template<typename Derived>
    //struct Base {
    //    void public_test(int a) {
    //        std::cout << "Public fnc: " << a << "\n";
    //    }

    //    void private_test(int a) {
    //        std::cout << "Private fnc: " << a << "\n";
    //    }
    //};

    
}

namespace v5 {
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
}

namespace v6 {
    template<typename Hooked>
    struct EventHookable {
    protected:
        EventHookable() {
            if constexpr (requires(Hooked h) { h.onCreated(); })
                static_cast<Hooked*>(this)->onCreated();
            else onCreatedDefault();

            std::cout << "System @" << this << '\n';

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
        { static_cast<Derived*>(this)->invokeUpdate(); }
    };

    struct Empty {
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

    #define SYSTEM_HOOKS(Derived, Next)           \
        UpdateLinkable<Derived, Next>,           \
        EventHookable<Derived>                  

    #define SYSTEM_FRIENDS(Derived, Next)         \
        friend EventHookable<Derived>;           \
        friend UpdateLinkable<Derived, Next>;

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

    public:
        Pipeline() {
            std::cout << "Creating new pipeline\n";
        }

        ~Pipeline() {
            std::cout << "Destroying pipeline\n";
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
}

namespace v7 {
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
}

namespace v8 {
    template<typename Hooked>
    struct EventHookable {
    protected:
        EventHookable() {
            if constexpr (requires(Hooked h) { h.onCreated(); })
                static_cast<Hooked*>(this)->onCreated();
            else onCreatedDefault();
        }

        ~EventHookable() {
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
        void onCreatedDefault() { /*std::cout << "No 'onCreated' using default.\n";*/ }
        void onDestroyedDefault() { /*std::cout << "No 'onDestroyed' using default.\n"; */ }

        void onUpdateDefault() { /*std::cout << "No 'onUpdate' using default.\n"; */ }
        void onPreUpdateDefault() { /*std::cout << "No 'onPreUpdate' using default.\n"; */ }
        void onPostUpdateDefault() { /*std::cout << "No 'onPostUpdate' using default.\n"; */ }

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
               : Singleton<World>(t) { }
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

    struct MockSystem : private WorldProxy {
    public:
        void update() {
            this->query();
        }

    private:


    };
}

namespace v9 {
    template<typename Hooked>
    struct EventHookable {
    protected:
        EventHookable() {
            if constexpr (requires(Hooked h) { h.onCreated(); })
                static_cast<Hooked*>(this)->onCreated();
            else onCreatedDefault();
        }

        ~EventHookable() {
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
        void onCreatedDefault() { /*std::cout << "No 'onCreated' using default.\n";*/ }
        void onDestroyedDefault() { /*std::cout << "No 'onDestroyed' using default.\n"; */ }

        void onUpdateDefault() { /*std::cout << "No 'onUpdate' using default.\n"; */ }
        void onPreUpdateDefault() { /*std::cout << "No 'onPreUpdate' using default.\n"; */ }
        void onPostUpdateDefault() { /*std::cout << "No 'onPostUpdate' using default.\n"; */ }

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
    struct LinkedSystem : SYSTEM_HOOKS(LinkedSystem<Next>, Next) {
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
}

int main() {
   
    {
        using namespace v1;

        //Object o;

        //o.extractSize<SizedObject>();
        //o.doSomething<int>();
    }
    
    {
        using namespace v2;

        //System sys;

        //sys.print_secret();
    
        
    }

    {
        using namespace v3;

        //System sys;

        //sys.print_secret();
    }

    {
        using namespace funcx;

        //Object obj;

        //Callable c;

        //c.call(obj, &Object::withArgs, 10, 50);

  
        std::vector<void (Object::*)()> actions;

        auto f = &Object::foo; actions.push_back(f);
        auto b = &Object::bar; actions.push_back(b);
        auto t = &Object::test; actions.push_back(t);

        //for (auto& a : actions) {
        //    //(obj.*a)();

        //    c.call(obj, a);
        //}

       


        // void (A::*)()
    }

    {
        using namespace v4;
        //Object obj;

        //Invoker::invoke(obj, &Object::public_test, 10);

        //Invoker::invoke(obj, &Object::private_test, 10);

        //Invoker invoker;

        //invoker.test();

        //Invoker2<Object> invoker2;
    }

    {
        using namespace v5;

        //System sys;

        //sys.update();
    }

    {
        using namespace v6;

        //System<System<System<Empty>>> sys;

        //PipelineExecutor<
        //    Pipeline<System<System<System<Empty>>>>
        //> pipeline_exec;

        //std::cout << "\n\n\n";


        //Pipeline<System<System<System<Empty>>>> a;
       
        //pipeline_exec.update_all();

        //sys.update();
    }

    {
        using namespace v7;

        //System<System<System<Empty>>> sys;

        //PipelineExecutor<
            //Pipeline<LinkedSystem<LinkedSystem<LinkedSystem<System>>>>
        //> pipeline_exec;

        //std::cout << "\n\n";


        //Pipeline<System<System<System<Empty>>>> a;

        //pipeline_exec.update_all();

        //std::cout << "\n\n";

        //sys.update();
    }

    {
        using namespace v8;

        //System<System<System<Empty>>> sys;

        //PipelineExecutor<
            //Pipeline<LinkedSystem<LinkedSystem<LinkedSystem<System>>>>
        //> pipeline_exec;

        //std::cout << "\n\n";


        //Pipeline<System<System<System<Empty>>>> a;

        //pipeline_exec.update_all();

        //std::cout << "\n\n";

        //sys.update();


        //MockSystem sys;

        //sys.update();

        //WorldProxy proxy;

        //proxy.emplace();
        //proxy.get();
        //proxy.query();
        //proxy.remove();

        //sys.update();
    }

    using namespace v9;

    //System<System<System<Empty>>> sys;

    //PipelineExecutor<
        //Pipeline<LinkedSystem<LinkedSystem<LinkedSystem<System>>>>
    //> pipeline_exec;

    //std::cout << "\n\n";


    //Pipeline<System<System<System<Empty>>>> a;

    //pipeline_exec.update_all();

    //std::cout << "\n\n";

    //sys.update();


    MockSystem sys;

    //sys.update();

    WorldProxy proxy;

    //proxy.emplace();
    //proxy.get();
    //proxy.query();
    //proxy.remove();

    sys.update();
}
