#include <mreflection.h>
#include <iostream>
#include <funcid>

class Ukaz : public MReflection::Instance {
public:
	struct Reflection : public MReflection {
		Reflection();
	};
	MReflection& getClass();
	void izpisi_stevilo ( int stevilo ) reflect(this,stevilo)
	void izpisi_a() reflect(this)
};

Ukaz::Reflection::Reflection() : MReflection ( static_cast<Ukaz*>(0) ) {
	funcs<void(void*,int)>() [method_name(Ukaz,izpisi_stevilo)] = [] ( void* _This, int stevilo ) { std::cerr << stevilo << std::endl; };
	funcs<void()>() [method_name(Ukaz,izpisi_a)] = [] () { std::cerr << "a" << std::endl; };
}

Ukaz::Reflection Class;

MReflection& Ukaz::getClass() {
	return Class;
}

int main ( int argc, char** argv ) {
	MReflection* reflection = MReflection::get ( "Ukaz" );
	MReflection::Instance* instance = reflection->newInstance();
	reflection->invoke ( "izpisi_stevilo(int)", instance, 2 );
	reflection->invoke ( "izpisi_a()", instance );
	instance->invoke ( "izpisi_stevilo(int)", 2 );
	instance->invoke ( "izpisi_a()" );
	dynamic_cast<Ukaz*> ( instance )->izpisi_stevilo ( 2 );
	dynamic_cast<Ukaz*> ( instance )->izpisi_a();
	return 0;
}
