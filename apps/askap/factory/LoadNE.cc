/// @file LoadNE.cc
///
/// @abstract
/// Derived from DaliugeApplication
/// @ details
/// Implements a test method that uses the contents of the the parset to load
/// in a measurement set and print a summary of its contents
///
// for logging
#define ASKAP_PACKAGE_NAME "LoadNE"
#include <string>
/// askap namespace
namespace askap {
/// @return version of the package
    std::string getAskapPackageVersion_LoadNE() {
        return std::string("LoadNE");
    }


} // namespace askap

/// The version of the package
#define ASKAP_PACKAGE_VERSION askap::getAskapPackageVersion_LoadNE()

#include <iostream>
#include <vector>



#include <daliuge/DaliugeApplication.h>
#include <factory/LoadNE.h>
#include <factory/NEUtils.h>

// LOFAR ParameterSet
#include <Common/ParameterSet.h>
// LOFAR Blob
#include <Blob/BlobString.h>
#include <Blob/BlobOBufString.h>
#include <Blob/BlobIBufString.h>
#include <Blob/BlobOStream.h>
#include <Blob/BlobIStream.h>
// ASKAP Logger

#include <askap/AskapLogging.h>
#include <askap/AskapError.h>

// Data accessors
#include <dataaccess/TableConstDataSource.h>
#include <dataaccess/IConstDataIterator.h>
#include <dataaccess/IDataConverter.h>
#include <dataaccess/IDataSelector.h>
#include <dataaccess/IDataIterator.h>
#include <dataaccess/SharedIter.h>

// params helpers
#include <measurementequation/SynthesisParamsHelper.h>
#include <measurementequation/ImageParamsHelper.h>
#include <measurementequation/ImageFFTEquation.h>
#include <parallel/GroupVisAggregator.h>


#include <gridding/IVisGridder.h>
#include <gridding/VisGridderFactory.h>


#include <fitting/Params.h>


#include <string.h>
#include <sys/time.h>




namespace askap {


  

    LoadNE::LoadNE() {
        //ASKAP_LOGGER(locallogger,"\t LoadNE -  default contructor\n");
        std::cout << "LoadNE -  default constructor" << std::endl;

    }


    LoadNE::~LoadNE() {
        //ASKAP_LOGGER(locallogger,"\t LoadNE -  default destructor\n");
        std::cout << "LoadNE -  default destructor" << std::endl;
    }

    DaliugeApplication::ShPtr LoadNE::createDaliugeApplication(const std::string &name)
    {
        // ASKAP_LOGGER(locallogger, ".create");
        std::cout << "createDaliugeApplication - Instantiating LoadNE" << std::endl;
        // ASKAPLOG_INFO_STR(locallogger,"createDaliugeApplication - Instantiating LoadNE");
        LoadNE::ShPtr ptr;

        // We need to pull all the parameters out of the parset - and set
        // all the private variables required to define the beam


        ptr.reset( new LoadNE());

        std::cout << "createDaliugeApplication - Created LoadNE DaliugeApplication instance " << std::endl;
        return ptr;

    }
    int LoadNE::init(dlg_app_info *app, const char ***arguments) {



        while (1) {

            const char **param = *arguments;

            // Sentinel
            if (param == NULL) {
                break;
            }

            arguments++;
        }

        app->data = malloc(sizeof(struct app_data));
        if (!app->data) {
            return 1;
        }


        return 0;
    }

    int LoadNE::run(dlg_app_info *app) {

        // Lets get the key-value-parset
        ASKAPLOG_INIT("");
        ASKAP_LOGGER(logger, ".run");

        askap::scimath::ImagingNormalEquations::ShPtr itsNe = askap::scimath::ImagingNormalEquations::ShPtr(new askap::scimath::ImagingNormalEquations());

        NEUtils::receiveNE(itsNe, app);

        std::vector<std::string> toFitParams = itsNe->unknowns();
        std::vector<std::string>::const_iterator iter2 = toFitParams.begin();
        for (; iter2 != toFitParams.end(); iter2++) {

            ASKAPLOG_INFO_STR(logger,"Param name: " << *iter2);
        }

        return 0;
    }


    void LoadNE::data_written(dlg_app_info *app, const char *uid,
        const char *data, size_t n) {

        app->running();

    }

    void LoadNE::drop_completed(dlg_app_info *app, const char *uid,
            drop_status status) {

        app->done(APP_FINISHED);
        delete(to_app_data(app)->parset);
    }


} // namespace
