#include <iostream>

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

        engine.invoke_position_update();


        //engine.update_all();

        //engine.update_all();
    
        ISystem<PositionSystem<Empty>> posys;

        posys.update();
    }
}
