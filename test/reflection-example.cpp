#include <mreflection.h>
#include <iostream>

class Ukaz {
    M_REFLECTION
public:
    int stevilo;
    void izpisi_stevilo ( int stevilo ) { std::cerr << stevilo << std::endl; }
    void izpisi_stevilo () { std::cerr << stevilo << std::endl; }
    static void izpisi(const char* s) { std::cerr << s << std::endl; }
};

extern "C" MReflection* Ukaz_REFLECTION() {
    static MReflection refl;
    refl.M_REFLECT_HELPER(Ukaz,new);
    refl.M_REFLECT_HELPER(Ukaz,delete);
    refl.M_REFLECT_METHOD(Ukaz,izpisi_stevilo,());
    refl.M_REFLECT_METHOD(Ukaz,izpisi_stevilo,(int));
    refl.M_REFLECT_STATIC(Ukaz,izpisi,(const char*));
    refl.M_REFLECT_FIELD(Ukaz,stevilo);
    return &refl;
}

int main ( int argc, char** argv ) {
    MReflection* reflection = MReflection::get ( "Ukaz" );
    void* instance = reflection->newInstance();
    reflection->setField ( instance, "stevilo", 4 );
    reflection->setField ( instance, "z1", 6 );
    std::cerr << reflection->getField<int> ( instance, "stevilo" ) << std::endl;
    std::cerr << reflection->getField<int> ( instance, "z2" ) << std::endl;
    reflection->invoke ( instance, "izpisi_stevilo(int)", 2 );
    reflection->invoke ( instance, "izpisi_stevilo()" );
    reflection->invoke_static ( "izpisi(const char*)", "a" );
    static_cast<Ukaz*>(instance)->invoke ( "izpisi_stevilo(int)", 2 );
    static_cast<Ukaz*>(instance)->invoke ( "izpisi_stevilo()" );
    static_cast<Ukaz*>(instance)->izpisi_stevilo(2);
    static_cast<Ukaz*>(instance)->izpisi_stevilo();
    reflection->deleteInstance(instance);
    return 0;
}
