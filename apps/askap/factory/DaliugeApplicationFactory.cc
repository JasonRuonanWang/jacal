/// @file DaliugeApplicationFactory.cc
///
/// @abstract
/// Factory class that registers and manages the different possible instances of
/// of a DaliugeApplication.
/// @ details
/// Maintains a registry of possible applications and selects - based upon a name
/// which one will be instantiated.
///

// ASKAPsoft includes


#include <askap/AskapError.h>
#include <casacore/casa/OS/DynLib.h>        // for dynamic library loading
#include <casacore/casa/BasicSL/String.h>   // for downcase
#include <boost/program_options.hpp>

// Local package includes

#include <daliuge/DaliugeApplication.h>
#include <factory/DaliugeApplicationFactory.h>

// Apps need to be here - or can we register them from Somewhere else
#include <factory/Example.h>
#include <factory/LoadParset.h>
#include <factory/LoadVis.h>
#include <factory/LoadNE.h>
#include <factory/SolveNE.h>
#include <factory/CalcNE.h>

//

#include<string>

namespace askap {


  // Define the static registry.
  std::map<std::string, DaliugeApplicationFactory::DaliugeApplicationCreator*>
  DaliugeApplicationFactory::theirRegistry;


  DaliugeApplicationFactory::DaliugeApplicationFactory() {
  }

  void DaliugeApplicationFactory::registerDaliugeApplication (const std::string& name,
                                           DaliugeApplicationFactory::DaliugeApplicationCreator* creatorFunc)
  {
    std::cout << "factory - Adding " << name.c_str() << " to the application registry" << std::endl;
    theirRegistry[name] = creatorFunc;
  }

  DaliugeApplication::ShPtr DaliugeApplicationFactory::createDaliugeApplication (const std::string& name)
  {
    std::cout << "factory - Attempting to find " <<  name.c_str() << " in the registry" << std::endl;
    std::map<std::string,DaliugeApplicationCreator*>::const_iterator it = theirRegistry.find (name);
    if (it == theirRegistry.end()) {
      // Unknown Application. Try to load from a dynamic library
      // with that lowercase name (without possible template extension).
      std::string libname(casa::downcase(name));
      const std::string::size_type pos = libname.find_first_of (".<");
      if (pos != std::string::npos) {
        libname = libname.substr (0, pos);      // only take before . or <
      }
      // Try to load the dynamic library and execute its register function.
      // Do not dlclose the library.
      std::cout << "factory - Application " << name.c_str() << " is not in the Daliuge Application registry, attempting to load it dynamically" << std::endl;

      casa::DynLib dl(libname, string("libaskap_"), "register_"+libname, false);
      if (dl.getHandle()) {
        // Successfully loaded. Get the creator function.
        std::cout << "factory - Dynamically loaded ASKAP/Daliuge Application " << name.c_str() << std::endl;
        // the first thing the Application in the shared library is supposed to do is to
        // register itself. Therefore, its name will appear in the registry.
        it = theirRegistry.find (name);
      }
    }
    if (it == theirRegistry.end()) {
      ASKAPTHROW(AskapError, "factory - Unknown Application " << name);
    }
    // Execute the registered function.
    return it->second(name);
  }

  // Make the Primary Beam object for the Primary Beam given in the parset file.
  // Currently the standard Beams are still handled by this function.
  // In the (near) future it should be done by putting creator functions
  // for these Beams in the registry and use that.

DaliugeApplication::ShPtr DaliugeApplicationFactory::make(const std::string &name) {

    if (theirRegistry.size() == 0) {
        // this is the first call of the method, we need to fill the registry with
        // all pre-defined applications
        std::cout << "factory - Filling the registry with predefined applications" << std::endl;
        addPreDefinedDaliugeApplication<Example>();
        addPreDefinedDaliugeApplication<LoadParset>();
        addPreDefinedDaliugeApplication<LoadVis>();
        addPreDefinedDaliugeApplication<LoadNE>();
        addPreDefinedDaliugeApplication<SolveNE>();



    }

    // buffer for the result
    DaliugeApplication::ShPtr App;

    App = createDaliugeApplication (name);

    ASKAPASSERT(App); // if a App of that name is in the registry it will be here

    return App;
}



} // namespace askap
