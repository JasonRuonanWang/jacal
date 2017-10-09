/// @file GaussianPB.cc
///
/// @abstract
/// Derived from PrimaryBeams this is the Gaussian beam as already implemented
/// @ details
/// Implements the methods that evaluate the primary beam gain ain the case of a
/// Gaussian
///


#include <daliuge/DaliugeApplication.h>
#include <factory/LoadParset.h>
#include <factory/NEUtils.h>


// LOFAR ParameterSet
#include <Common/ParameterSet.h>

#include <string.h>
#include <sys/time.h>


namespace askap {

    LoadParset::LoadParset() {
        std::cout << "LoadParset -  default contructor" << std::endl;
    }


    LoadParset::~LoadParset() {
        std::cout << "LoadParset -  default destructor" << std::endl;
    }

    DaliugeApplication::ShPtr LoadParset::createDaliugeApplication(const std::string &name)
    {
        std::cout << "createDaliugeApplication - Instantiating LoadParset" << std::endl;

        LoadParset::ShPtr ptr;

        // We need to pull all the parameters out of the parset - and set
        // all the private variables required to define the beam


        ptr.reset( new LoadParset());

        std::cout << "createDaliugeApplication - Created LoadParset DaliugeApplication instance" << std::endl;
        return ptr;

    }
    int LoadParset::init(dlg_app_info *app, const char ***arguments) {

        // std::cerr << "Hello World from init method" << std::endl;

        // Argument parsing is not working as yet


        char *parset_filename = 0;
        while (1) {

            const char **param = *arguments;

            // Sentinel
            if (param == NULL) {
                break;
            }
            // any params I might need go here:
            // filename:
            // no longer required as input comes from daliuge now
            //if (strcmp(param[0], "parset_filename") == 0) {
            //    parset_filename = strdup(param[1]);
            //}

            arguments++;
        }

        app->data = malloc(sizeof(struct app_data));
        if (!app->data) {
            return 1;
        }
        //  FIXME:
        //    This should be here but I could not get a boost smart pointer to work
        //    to_app_data(app)->parset.reset( new LOFAR::ParameterSet(parset_filename));
        //
        return 0;
    }

    int LoadParset::run(dlg_app_info *app) {

        // load the parset and print it out to the screen
        // std::cerr << "Hello World from run method" << std::endl;
    //    std::cout << *to_app_data(app)->parset << std::endl;

    // lets open the input and read it
        char buf[64*1024];
        size_t n_read = app->inputs[0].read(buf, 64*1024);

        to_app_data(app)->parset = new LOFAR::ParameterSet(true);
        to_app_data(app)->parset->adoptBuffer(buf);

        // write it to the outputs
        std::cout << *(to_app_data(app)->parset) << std::endl;


        for (int i = 0; i < app->n_outputs; i++) {
            app->outputs[i].write(buf, n_read);
        }

        return 0;
    }


    void LoadParset::data_written(dlg_app_info *app, const char *uid,
        const char *data, size_t n) {

        app->running();

    }

    void LoadParset::drop_completed(dlg_app_info *app, const char *uid,
            drop_status status) {

        app->done(APP_FINISHED);
        delete(to_app_data(app)->parset);
    }


} // namespace
