#include <iostream>
#include <tuple>
#include <functional>
#include <type_traits>


namespace GMixin {
    template<typename U>
    struct Logger {
        void log(const U& e) const noexcept {
            std::cout << e << "\n";
        }
    };

    template<template<typename> class Mixin, typename T>
    struct Wrapper : Mixin<T> {
        void hello() const noexcept {
            std::cout << "Called from the Wrapper!\n";
        }
    };
}

namespace DMixin {
    template<typename U>
    struct Logger {
        void log(const U& e) const noexcept {
            std::cout << e << "\n";
        }
    };

    template<typename Derived>
    struct GenericMixin {
        void log() const noexcept {
            std::cout << "[Generic Mixin] Running Derived...\n";
            static_cast<Derived*>(this)->run();
        }
    };

    template<template<typename> class... Mixins>
    struct Layer : Mixins<int>... {
        void MIXIN_print() const noexcept {
            std::cout << "Printing mixin...\n";
        }
    };

    template<template<typename> class... Mixins>
    struct MultiLayer : Mixins<int>... {
        void MIXIN_print() const noexcept {
            std::cout << "Printing mixin...\n";
        }
    };

    struct with {
        void do_something() const noexcept {
            std::cout << "something from with!\n";
        }
    };

    template<typename Derived>
    struct Propagator {
        template<typename... Ts>
        void of() const noexcept {

        }


        with with{};
    };

    /*template<template<typename> typename... Ts>
    struct Reciever : Ts<Reciever>... {

    };*/

    template<typename... Args>
    class Base : Args... {
    public:
        //using Types = Args;

    };


    //recursive mixin cluster
    template<template<typename> typename... Ts>
    struct Derived : Base<Ts<Derived<Ts...>>...> {
        // Derived herite de Base
        // Base herite de tous les arguments
        // de Ts
        // Tous les arguments de Ts héritent de base
        // 
        // De cette facon, on peut faire du crtp sur TOUS
        // les membres envoyés dans Derived en partant de base
        // par exemple, une base pourrait dispatch une information
        // ou un exécution sur tout l'arbre de crtp. 
        // Comme si tout était connecté

        /*Derived
          ↑
        Base<T1<Base>, T2<Base>, T3<Base>, ...>
          // Derived hérite de ça
          ↑         ↑          ↑
        T1<Base>  T2<Base>   T3<Base>   // grâce à : Args... dans Base
          ↑         ↑          ↑
         Base      Base       Base      // grâce au CRTP dans chaque Ti<Base>*/
    
    };

    template<template<typename...> typename... Ts>
    struct Mutual : Ts<int, float>... {

    };

    template<template<typename> class Mixin, typename T>
    struct Wrapper : Mixin<T> {
        void hello() const noexcept {
            std::cout << "Called from the Wrapper!\n";
        }
    };


    struct SimpleObject {
        void print() const noexcept {
            std::cout << "Test\n";
        }
    };

    template<typename T>
    struct MetaObject {
    public:
        void META_OBJECT_print() const noexcept {
            std::cout << data_ << "\n";
        }

    private:
        T data_{};
    };
}

namespace xyx {
    template<typename Base>
    struct MixinA : Base {
        void testA() const noexcept {}
    };

    template<typename Base>
    struct MixinB : Base {
        void testB() const noexcept {}
    };

    /********************/

    template<typename... Ts>
    struct Base : Ts... {
        void test_base() const noexcept {

        }
    };

    /*template<template<typename> typename... Ts>
    struct Derived : Base<Ts<Derived<Ts...>>...> {
        void test_derived() const noexcept {

        }
    };*/

    //template<template<typename> typename... Ts>
    //struct Derived : Base<Ts<Base>...> {

    //};
}

namespace zyz {
    struct Empty { void tick() {} };

    template<typename Next>
    struct MixinA { void tick() { std::cout << "A\n"; } };
    template<typename Next>
    struct MixinB { void tick() { std::cout << "B\n"; } };
    template<typename Next>
    struct MixinC { void tick() { std::cout << "C\n"; } };

    template<template<typename> class... Layers>
    struct Engine : Layers<Engine<Layers...>>..., Empty {
        void tick() {
            (static_cast<Layers<Engine>*>(this)->tick(), ...);

            // en gros -> MixinA<Engine<MixinA, MixinB, MixinC>>
            //            MixinB<Engine<MixinA, MixinB, MixinC>>
            //            MixinC<Engine<MixinA, MixinB, MixinC>>

            //imagine comment ça serait insane de mélanger du sfinae tmp 
            //pour n'apeller que les layers qui arborent les méthodes
            //appelées pour pouvoir tout condenser a faire du dispatch
            //"dynamic" tout en restant 100% static ahahahahah
        }

        void tick_first() const noexcept {}
    };
}

namespace xyz {
    struct Empty { void tick() {} };

    template<typename Next>
    struct MixinA { void tick() { std::cout << "A\n"; } };
    template<typename Next>
    struct MixinB { void tick() { std::cout << "B\n"; } };
    template<typename Next>
    struct MixinC { void tick() { std::cout << "C\n"; } };

    template<template<typename> class... Layers>
    struct Engine : Layers<Engine<Layers...>>..., Empty{
    public:
        Engine() : layers_(Layers<Engine<Layers...>>()...) {}

    public:
        void tick_all() {
            (static_cast<Layers<Engine>*>(this)->tick(), ...);
            
            /* on pourrait faire en sorte que j'aie un mixin stack 
               avec une classe maitre qui contient le registre ecs 
               et qui update les systems voulus (systems sont les 
               variadic injectés dans engine)*/
        }

        void tick_first() noexcept {
            std::get<0>(layers_).tick();
        }

        template<template<typename> class Layer>
        auto tick() noexcept ->
            std::enable_if_t<std::disjunction_v<
            std::is_same<Layer<Engine>, Layers<Engine>>...>,
        void> {
            std::get<Layer<Engine>>(layers_).tick();
        }

        template<template<typename> class... QLayer>
        auto tick_pack() noexcept -> void
            /*std::enable_if_t<
                std::disjunction_v<std::is_same<QLayer<Engine>, Layers<Engine>>...>
            , void>*/
        {
        }

        template<template<typename> class Layer>
        auto print_element_of() const noexcept ->
            std::enable_if_t<std::disjunction_v<
            std::is_same<Layer<Engine>, Layers<Engine>>...>,
        void> {
            std::cout << typeid(std::get<Layer<Engine>>(layers_)).name()
                      << "\n";
        }

    private:
        std::tuple<Layers<Engine<Layers...>>...> layers_;
    };
}

namespace ecsave {
    //template<
    //    //typename T,
    //    template<typename> class... SystemLayers
    //>
    //struct Engine :
    //    CoreInvokerMixin<Engine<SystemLayers...>>,
    //    SystemLayers<Engine<SystemLayers...>>... {
    //public:
    //    Engine() : layers_(SystemLayers<Engine<SystemLayers...>>()...) {}

    //public:
    //    //auto update_all() noexcept {
    //        //(static_cast<SystemLayers<Engine>*>(this)->update(), ...);
    //    //}

    //private:
    //    std::tuple<SystemLayers<Engine<SystemLayers...>>...> layers_;
    //    // pas forcément utile, DELETER SI TROUVE PAS DE BUT
    //    // rapelle toi que cette classe ne devrait pas gerer trop
    //    // de trucs différents, reste au minimum

    //};
}

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
        //genre faire des lambdas injectables
        //on fait un genre d'index et le core invoker
        //n'est que le maitre qui connait l'index dans la 
        //liste de lambdas injectés dans Engine. La liste de lambdas 
        //engine est private mais elle est accessible via friend.
        //core invoker INVOKERA avec le CRTP les bonnes fonctions 
        //correspondantes aux indexes dans la liste private de Engine

        //ensuite, avec sfinae, on filtre et on accepte uniquement
        //ce qui fait du sens, genre pas injecter un lambdas qui utilise
        //un system qui n'est pas la.

        /*auto lambda = [this]() { 
            static_cast<Derived*>(this)->positionSystem.updatePositions();
        };
        */

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

namespace ecss {
    struct Empty {
        void on_update() noexcept {}
    };

    template<typename DerivedSystem>
    struct ISystem {
    public:
        void update() noexcept {
            static_cast<DerivedSystem*>(this)->proxy_update()();
        }
    };

    template<typename Next = Empty>
    struct PositionSystem
        : public Next
        , public ISystem<PositionSystem<Next>> {
    public:
        friend struct ISystem<PositionSystem<Next>>;

    protected:
        void on_update() {
            std::cout << "[PositionSystem] Updating...\n";
            std::cout << "Next: "
                      << typeid(Next).name() << "\n";
        }

    private:
        auto proxy_update() noexcept {
            return [this]() { return on_update(); };
        }
    };

    template<typename Next = Empty>
    struct VelocitySystem
        : public Next
        , public ISystem<VelocitySystem<Next>> {
    public:
        friend struct ISystem<VelocitySystem<Next>>;

    protected:
        void on_update() {
            std::cout << "[VelocitySystem] Updating...\n";
            std::cout << "Next: "
                      << typeid(Next).name() << "\n";

            //static_cast<Next*>(this)->update();
        }

    private:
        auto proxy_update() noexcept {
            return [this]() { return on_update(); };
        }
    };

    template<typename Core>
    struct ProxyInvoker {
        void invoke_print() {
            static_cast<Core*>(this)->print();
        }

        template<typename System>
        auto invoke_update() noexcept {
            static_cast<System*>(this)->update();
        }
    };

    template<template<typename> class... Layers>
    struct Engine : ProxyInvoker<Engine<Layers...>>,
                    protected Layers<Engine<Layers...>>...
    {               friend struct ProxyInvoker<Engine>;
    private:
        void print() {
            std::cout << "Printing Engine!\n";
        }
    };

    /*template<template<typename> class... Layers>
    struct Engine : Invoker<Engine<Layers...>>,
                    Layers<Engine<Layers...>>...
    {
    public:
        friend struct Invoker<Engine>;

    private:
        void print() {
            std::cout << "Printing Engine!\n";
        }
    };*/
}

namespace stack {
    struct Empty {
        void update() { std::cout << "[Empty] Updating last...\n"; }
    };

    template<typename Next = Empty>
    struct PositionSystem : Next {
        void update() {
            std::cout << "[PositionSystem] Updating...\n";
            Next::update();
        }
    };

    template<typename Next = Empty>
    struct VelocitySystem : Next {
        void update() {
            std::cout << "[VelocitySystem] Updating...\n";
            Next::update();
        }
    };
}

namespace cluster {
    template<typename Derived>
    struct ISystem {
        void update() {
            static_cast<Derived*>(this)->on_update();
        }
    };

    template<typename Next>
    struct PositionSystem : public ISystem<PositionSystem<Next>> {
    public:
        void on_update() {
            std::cout << "[PositionSystem] Updating...\n";
        }
    };

    template<typename Next>
    struct VelocitySystem : ISystem<VelocitySystem<Next>> {
    public:
        void on_update() {
            std::cout << "[VelocitySystem] Updating...\n";
        }
    };

    template<typename Next>
    struct RenderingSystem : public ISystem<RenderingSystem<Next>> {
    public:
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
    template<typename Next> // next est useless // faire = void, pas empty
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

namespace engine4 {
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
    template<typename Next> // next est useless // faire = void, pas empty
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
    using OldPipeline = LayeredPipeline<Layers...>;

    /*template<template<typename> class Outer, typename Inner>
    struct Pipeline<Outer<Inner>> {
        static void print() {
            std::cout << "Outer: " << typeid(Outer<Inner>).name() << "\n";
            Pipeline<Inner>::print();
        }
    };*/

    /*template <typename InnerLayer, template<typename> class... OuterLayers>
    struct Pipeline<OuterLayers<InnerLayer>...> {
        static void print() {
            static std::size_t is = 0;

            std::cout << "Outer: "
                      << typeid(OuterLayers<InnerLayer>).name() << "\n";
            ++is;
            std::cout << is << "\n";
            Pipeline<InnerLayer>::print();

           
           
        }
    };*/

    //template<typename... Layers>
    //struct Pipeline {
    //    static void print() {
    //        static std::size_t i = 0;
    //        ((std::cout << typeid(Ts).name() << "\n"), ...);
    //        ++i;
    //        std::cout << i << "\n";
    //    }
    //};

    //template<typename First, typename... Rest>
    //void trace(First&& first, Rest&& ... rest) {
    //    if constexpr (sizeof...(rest) == 0) {
    //        std::cout << std::forward<First>(first) << '\n';
    //    }
    //    else {
    //        std::cout << std::forward<First>(first) << ' ';
    //        trace(std::forward<Rest>(rest)...);
    //    }
    //}

    //template<typename... Args>
    //void trace2(Args&& ... args) {
    //    std::cerr << "Error: ";
    //    ((std::cerr << args << ' '), ...) << '\n';
    //    // NOTE: unlike first version, there is an extra comma at the end
    //}

    /****************************************/

    template<typename T>
    struct Wrapper {
        using type = T;
    };

    template<typename... Ts>
    struct MultiWrapper {
        using type = std::tuple<Ts...>;
    };
}

namespace engine5 {
    struct Empty {
        void update() { std::cout << "[Empty] Updating last...\n\n"; }
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
    template<typename Next> // next est useless // faire = void, pas empty
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

    /*****************************************************/
    /*****************************************************/

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
    struct Pipeline {
        //faire un tuple qui contient des pipelines différents.

        //chaque elements du tuple est son propre pipeline et il contient
        //toute la stack d'un des éléments de typename... Ts

        //lorsqu'on veut caller on specific pipeline, on peux apeller
        //all et mettre template et sfinae pour specifier quel 
        //pipeline déclencher.

    public:
        Pipeline() : pipelines_(First{}, Rest{}...) {}


    public:
        void test() {
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
    };
}

namespace engine6 {
    struct Empty {
        void update() { std::cout << "[Empty] Updating last...\n\n"; }
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
    template<typename Next> // next est useless // faire = void, pas empty
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

    /*****************************************************/
    /*****************************************************/

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
    struct PipelineDispatcher {


    public:
        PipelineDispatcher() : pipelines_(First{}, Rest{}...) {}

    public:
        void update_all() noexcept {
            std::apply([](auto&&... args) {
                //args.pipeline_
                ((std::cout << typeid(args.layers_).name() << std::endl), ...);

                ((args.update()), ...);
            }, pipelines_);
        }

    private:
        std::tuple<First, Rest...> pipelines_;
    };

    template<
        typename Layers,
        template<typename> class Owner = PipelineDispatcher
    > struct Pipeline : private Layers {
        using type = Layers;
        friend Owner<Pipeline<Layers>>;

    protected:
        Layers layers_; // possiblement delete

    };


    template<typename First, typename... Rest>
    struct PipelineManager {
        //faire un tuple qui contient des pipelines différents.

        //chaque elements du tuple est son propre pipeline et il contient
        //toute la stack d'un des éléments de typename... Ts

        //lorsqu'on veut caller on specific pipeline, on peux apeller
        //all et mettre template et sfinae pour specifier quel 
        //pipeline déclencher.

    public:
        PipelineManager() : pipelines_(First{}, Rest{}...) {}


    public:
        void update_all() {
            std::apply([](auto&&... args) {
                ((args.update()), ...);
            }, pipelines_);
        }

        // probablement bouger, juste pour test
        //peut etre faire un truc avec lambda, voir...
        template<typename T>
        auto update() /*noexcept*/ ->
        std::enable_if_t<
            std::is_same_v<T, First> ||
            (std::is_same_v<T, Rest> && ...),
        void> {
            std::cout << "[Updating System]\n";

            //une copie... bad
            std::get<T>(pipelines_).update();
        }

    private:
        std::tuple<First, Rest...> pipelines_;
    };

    template<typename... Components>
    struct Engine : Components... {
        //genre sfinae {if un des variadic est un pipeline, iterer}

        //normalement, je penses pas qu'il y aura tant de code que ca ici
        //il devrait hériter des components qui sont des specialisations
    };
}

namespace engine7 {
    struct Empty {
        void update() { std::cout << "[Empty] Updating last...\n\n"; }
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
    template<typename Next> // next est useless // faire = void, pas empty
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

    /*****************************************************/
    /*****************************************************/

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

    template <typename... Pipelines>
    struct PipelineDispatcher {
    private:
        std::tuple<Pipelines...> pipelines_;

    public:
        PipelineDispatcher() : pipelines_(Pipelines{}...) {}

        void update_all() noexcept {
            // Utilisation de std::index_sequence pour itérer sur le tuple
            update_impl(std::index_sequence_for<Pipelines...>{});
        }

    private:
        template <std::size_t... I>
        void update_impl(std::index_sequence<I...>) noexcept {
            // Accès aux éléments du tuple via l'indexation
            ((std::get<I>(pipelines_).update()), ...);
        }
    };

    template<typename T>
    struct Pipeline : T {
        T layers_;

        // Permet à PipelineDispatcher d'accéder aux membres privés
        template<typename...>
        friend struct PipelineDispatcher;
    };

   /* template<typename Layers>
    struct Pipeline2 : private Layers {

    };*/


    template<typename First, typename... Rest>
    struct PipelineManager {
        //faire un tuple qui contient des pipelines différents.

        //chaque elements du tuple est son propre pipeline et il contient
        //toute la stack d'un des éléments de typename... Ts

        //lorsqu'on veut caller on specific pipeline, on peux apeller
        //all et mettre template et sfinae pour specifier quel 
        //pipeline déclencher.

    public:
        PipelineManager() : pipelines_(First{}, Rest{}...) {}


    public:
        void update_all() {
            std::apply([](auto&&... args) {
                ((args.update()), ...);
            }, pipelines_);
        }

        // probablement bouger, juste pour test
        //peut etre faire un truc avec lambda, voir...
        template<typename T>
        auto update() /*noexcept*/ ->
        std::enable_if_t<
            std::is_same_v<T, First> ||
            (std::is_same_v<T, Rest> && ...),
        void> {
            std::cout << "[Updating System]\n";

            //une copie... bad
            std::get<T>(pipelines_).update();
        }

    private:
        std::tuple<First, Rest...> pipelines_;
    };

    template<typename... Components>
    struct Engine : Components... {
        //genre sfinae {if un des variadic est un pipeline, iterer}

        //normalement, je penses pas qu'il y aura tant de code que ca ici
        //il devrait hériter des components qui sont des specialisations
    };
}

namespace engine8 {
    struct Empty {
        void update() { std::cout << "[Empty] Updating last...\n\n"; }
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
    template<typename Next> // next est useless // faire = void, pas empty
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

    /*****************************************************/
    /*****************************************************/

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

        //a la limite, faire un system qui passe registry sur toutes les étages
    };
}

namespace Hooks {
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

//template<typename T, T... Chars>
//std::string operator"" _test(const Chars )

int main() {
    {
        using namespace GMixin;

        Wrapper<Logger, int> w;

        //w.log(42);
        //w.hello();
    }

    {
        using namespace DMixin;

        //Base<SimpleObject> m;

        //m.print();

        /*****************/

        Layer<MetaObject> l;

        //l.META_OBJECT_print();
        //l.MIXIN_print();
    }

    {
        using namespace DMixin;

        Propagator<int> propagator;
    
        //propagator.with
            //.do_something();

       //propagator.of<int, float, double, bool>();
    }

    {
        using namespace xyx;

        //Derived<MixinA, MixinB> m;
    }

    {
        using namespace zyz;

        Engine<MixinA, MixinB, MixinC> engine;
         
        //engine.tick();
    }

    {
        using namespace xyz;

        Engine<MixinA, MixinB, MixinC> engine;

        //engine.tick<MixinA>();

        //engine.tick_all();

        //engine.print_element_of<MixinA>();
    }

    {
        using namespace ecs;
        
        Engine<PositionSystem> engine;

        //engine.invoke_position_update();


        //engine.update_all();

        //engine.update_all();
    
        ISystem<PositionSystem<Empty>> posys;

        //posys.update();
    }

    {
        using namespace ecss;

        Engine<> engine;

        //VelocitySystem<PositionSystem<>> velsys;
        PositionSystem possys;

        //velsys.update();

        //possys.update();

        //engine.invoke_print();

        //engine.invoke_update<PositionSystem>();
    }

    {
        using namespace stack;

        PositionSystem<
            VelocitySystem<Empty>
        > posys; //posys.update();

        VelocitySystem<
            PositionSystem<Empty>
        > velsys; //velsys.update();
    }

    {
        using namespace cluster;

        Pipeline<
            PositionSystem,
            VelocitySystem,
            RenderingSystem
        > pipeline;

        //pipeline.update_all();
    }

    {
        using namespace engine3;

        Pipeline<
            PositionSystem, VelocitySystem
        > pipeline;

        PositionSystem<VelocitySystem<Empty>> posys;

        //posys.update();

        //posys.info();

        //pipeline.update_all();

        //posys.info();

        //posys.info();
    }

    {
        //using namespace engine4;

        //OldPipeline<
        //    PositionSystem,
        //    VelocitySystem
        //> pipeline;

        //PositionSystem<VelocitySystem<Empty>> posys;

        ////posys.update();

        ////pipeline.update_all();

        //posys.info();

        //posys.info();

        ///*Pipeline<
        //    PositionSystem<VelocitySystem<void>>,
        //    VelocitySystem<PositionSystem<void>>
        //> t;

        //t.print();*/

        //MultiWrapper<
        //    PositionSystem<VelocitySystem<void>>,
        //    VelocitySystem<PositionSystem<void>>
        //> wrapper;

        //std::cout << typeid(Wrapper<PositionSystem<Empty>>::type).name() << "\n";

        //using Wrapped = MultiWrapper<
        //    PositionSystem<VelocitySystem<void>>,
        //    VelocitySystem<PositionSystem<void>>
        //>::type;

        //template<typename Wrapper>
        //struct Stack {
        //    void print() {

        //    }
        //};

        //std::cout << typeid(Wrapped).name() << "\n";
    }

    {
        using namespace engine5;

        using NetworkPipeline = ComponentA<ComponentB<ComponentB<ComponentA<Empty>>>>;
        using GraphicPipeline = ComponentA<ComponentB<ComponentA<Empty>>>;
        using PhysicsPipeline = ComponentA<ComponentA<ComponentB<Empty>>>;
        using AudioPipeline = ComponentB<ComponentB<ComponentA<Empty>>>;
        using UIPipeline = ComponentB<ComponentA<Empty>>;


        Pipeline<
            GraphicPipeline,
            AudioPipeline
        > stacked_pipeline;

        Engine<
            Pipeline<
                NetworkPipeline,
                GraphicPipeline,
                PhysicsPipeline,
                AudioPipeline,
                UIPipeline
            >
        > engine;

        //engine.test();

        /*
            class Engine {
                GraphicSystem gfx;
                PhysicsSystem phys;
                NetworkSystem ntws;
                AudioSystem audio;
                UiSystem ui;
            };
        */



        //stacked_pipeline.test();

        ComponentA<ComponentB<Empty>> stack;

        //stack.update();

        //stack.update();

        //stack.update();

        //posys.update();

        //posys.info();

        //pipeline.update_all();

        //posys.info();

        //posys.info();
    }

    {
        using namespace engine6;

        using NetworkPipeline = ComponentA<ComponentB<ComponentB<ComponentA<Empty>>>>;
        using GraphicPipeline = ComponentA<ComponentB<ComponentA<Empty>>>;
        using PhysicsPipeline = ComponentA<ComponentA<ComponentB<Empty>>>;
        using AudioPipeline = ComponentB<ComponentB<ComponentA<Empty>>>;
        using UIPipeline = ComponentB<ComponentA<Empty>>;


        Pipeline<
            ComponentA<ComponentB<ComponentB<ComponentA<Empty>>>>
        > networkPipeline;

        //networkPipeline.update();

        Pipeline<
            ComponentA<ComponentB<ComponentA<Empty>>>
        > graphicPipeline;

        PipelineDispatcher<
            Pipeline<ComponentA<ComponentB<ComponentA<Empty>>>>
        > pipelineDispatcher;

        //pipelineDispatcher.update_all();

        //graphicPipeline.update();

        PipelineManager<
            GraphicPipeline,
            AudioPipeline
        > stacked_pipeline;

        // GENRE LA CA VA PAS, PIPELINE -> GRAPHICPIPELINE, AUDIOPIPELINE
        // EN FAITE, FAUDRAIT PEUT ETRE FAIRE GENRE
        // J'AI UN PIPELINE QUI PRENDS UNE SEULE CHAINE.

        //ENSUITE J'AI COMME UN SYSTEM QUI WRAP TOUS LES PIPELINES EN NOMS
        //ET JE MET CA DANS ENGINE.
        //

        //stacked_pipeline.update();

        //stacked_pipeline.update<GraphicPipeline>();
        //stacked_pipeline.update<AudioPipeline>();


        Engine<
            PipelineManager <
                NetworkPipeline,
                GraphicPipeline,
                PhysicsPipeline,
                AudioPipeline,
                UIPipeline
            >
        > engine;

        //engine.update_all();

        /*
            class Engine {
                GraphicSystem gfx;
                PhysicsSystem phys;
                NetworkSystem ntws;
                AudioSystem audio;
                UiSystem ui;
            };
        */



        //stacked_pipeline.test();

        ComponentA<ComponentB<Empty>> stack;
    }

    {
        using namespace engine7;

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

        PipelineDispatcher<
            Pipeline<ComponentA<ComponentB<ComponentA<Empty>>>>,
            Pipeline<ComponentB<ComponentA<Empty>>>
        > pipelineDispatcher1;

        //pipelineDispatcher1.update_all();


        PipelineDispatcher<
            Pipeline<ComponentB<ComponentA<Empty>>>
        > pipelineDispatcher2;

        //pipelineDispatcher1.update_all();
        //pipelineDispatcher2.update_all();

        //graphicPipeline.update();

        PipelineManager<
            GraphicPipeline,
            AudioPipeline
        > stacked_pipeline;

        // GENRE LA CA VA PAS, PIPELINE -> GRAPHICPIPELINE, AUDIOPIPELINE
        // EN FAITE, FAUDRAIT PEUT ETRE FAIRE GENRE
        // J'AI UN PIPELINE QUI PRENDS UNE SEULE CHAINE.

        //ENSUITE J'AI COMME UN SYSTEM QUI WRAP TOUS LES PIPELINES EN NOMS
        //ET JE MET CA DANS ENGINE.
        //

        //stacked_pipeline.update();

        //stacked_pipeline.update<GraphicPipeline>();
        //stacked_pipeline.update<AudioPipeline>();


        Engine<
            PipelineManager <
            NetworkPipeline,
            GraphicPipeline,
            PhysicsPipeline,
            AudioPipeline,
            UIPipeline
            >
        > engine;

        //engine.update_all();

        /*
            class Engine {
                GraphicSystem gfx;
                PhysicsSystem phys;
                NetworkSystem ntws;
                AudioSystem audio;
                UiSystem ui;
            };
        */



        //stacked_pipeline.test();

        ComponentA<ComponentB<Empty>> stack;
    }

    {
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

        //engine.update_all();
    }

    {
        using namespace Hooks;

        //Derived derived;

        testDerived test;

        test.update();

        //derived.foo(5);

        //derived.foo(5.0f);

        //derived.foo(true);

        //derived.onCreated();

    }
}
