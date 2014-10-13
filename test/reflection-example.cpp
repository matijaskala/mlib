#include <mreflection.h>
#include <iostream>

class Ukaz {
public:
    struct Reflection;
    int stevilo;
    static MReflection& getClass();
    void izpisi_stevilo ( int stevilo ) { std::cerr << stevilo << std::endl; }
    void izpisi_stevilo () { std::cerr << stevilo << std::endl; }
        template< typename... _Args >
        void invoke ( const std::string& function_name, _Args... __args ) {
            getClass().invoke(function_name, this, __args...);
        }
};

static struct Ukaz::Reflection : public MReflection {
    Reflection() : M_REFLECTION_CONSTRUCT(Ukaz) {
        M_REFLECT_METHOD(Ukaz,izpisi_stevilo,());
        M_REFLECT_METHOD(Ukaz,izpisi_stevilo,(int));
        M_REFLECT_FIELD(Ukaz,stevilo);
    }
} Class;

MReflection& Ukaz::getClass() {
	return Class;
}

int main ( int argc, char** argv ) {
    MReflection* reflection = MReflection::get ( "Ukaz" );
    Ukaz* instance = reflection->newInstance<Ukaz>();
    reflection->access<int> ( instance, "z" ) = 6;
    reflection->invoke ( "izpisi_stevilo(int)", instance, 2 );
    reflection->invoke ( "izpisi_stevilo()", instance );
    instance->invoke ( "izpisi_stevilo(int)", 2 );
    instance->invoke ( "izpisi_stevilo()" );
    instance->izpisi_stevilo(2);
    instance->izpisi_stevilo();
    return 0;
}
