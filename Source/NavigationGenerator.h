#ifndef __NavigationGenerator_h__
#define __NavigationGenerator_h__

//class that generates the navigation meshes (not in runtime)
//this class will comprime Sample and Sample_SoloMesh.cpp
class NavigationGenerator
{
public:
	NavigationGenerator();
	~NavigationGenerator();

private:
	// Explicitly disabled copy constructor and copy assignment operator.
	NavigationGenerator(const NavigationGenerator&);
	NavigationGenerator& operator=(const NavigationGenerator&);
};

#endif