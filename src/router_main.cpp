/***************************************************************************
                          main.cpp
			  The main procedure for the build of person/vehicle routes
                             -------------------
    project              : SUMO
    subproject           : router
    begin                : Thu, 06 Jun 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] = "";
}
// $Log$
// Revision 1.9  2003/03/18 13:06:19  dkrajzew
// windows eol removed
//
// Revision 1.8  2003/03/12 16:34:34  dkrajzew
// some style guides applied
//
// Revision 1.7  2003/03/03 15:08:20  dkrajzew
// debugging
//
// Revision 1.6  2003/02/07 10:37:30  dkrajzew
// files updated
//
// Revision 1.4  2002/10/22 10:02:47  dkrajzew
// minor warnings removed
//
// Revision 1.3  2002/10/21 10:01:03  dkrajzew
// routedefs renamed to tripdefs in options
//
// Revision 1.2  2002/10/18 11:35:53  dkrajzew
// postinitialisation of edges for computation of lane-independent value added
//
// Revision 1.1  2002/10/16 14:51:08  dkrajzew
// Moved from ROOT/sumo to ROOT/src; added further help and main files for netconvert, router, od2trips and gui version
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>
#include <utils/convert/TplConvert.h>
#include <iostream>
#include <string>
#include <fstream>
#include <limits.h>
#include "router/ROLoader.h"
#include "router/ROLoader.h"
#include "router/RONet.h"
#include "utils/common/SErrorHandler.h"
#include "utils/options/Option.h"
#include "utils/options/OptionsCont.h"
#include "utils/options/OptionsIO.h"
#include "utils/common/UtilExceptions.h"
#include "utils/common/HelpPrinter.h"
#include "utils/xml/XMLSubSys.h"
#include "router_help.h"

/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
#endif
#endif



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * functions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * computation methods
 * ----------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/** validate options (settings) */
bool checkSettings(OptionsCont *oc)
{
    // check whether the output is valid and can be build
    if(!oc->isSet("o")) {
        cout << "No output specified." << endl;
        return false;
    }
    std::ofstream tst(oc->getString("o").c_str());
    if(!tst.good()) {
        cout << "The output file '" << oc->getString("o") << "' can not be build." << endl;
        return false;
    }
    //
    return true;
}


/** build and retrieve the options (settings) */
OptionsCont *
getSettings(int argc, char **argv)
{
    OptionsCont *oc = new OptionsCont();
    // register the file i/o options
    oc->doRegister("cell-input", new Option_FileName());
    oc->doRegister("artemis-input", new Option_FileName());
    oc->doRegister("output", 'o', new Option_FileName());
    oc->doRegister("net-files", 'n', new Option_FileName());
    oc->doRegister("weights", 'w', new Option_FileName());
    oc->doRegister("sumo-input", 's', new Option_FileName());
    oc->doRegister("trip-defs", 't', new Option_FileName());
    oc->doRegister("alternatives", 'a', new Option_FileName());
    oc->doRegister("configuration-file", 'c', new Option_FileName("sumo-router.cfg"));
    oc->addSynonyme("net-files", "net");
    oc->addSynonyme("output-file", "output");
    oc->addSynonyme("configuration-file", "configuration");
    oc->addSynonyme("weights", "weight-files");
    // register the simulation settings
    oc->doRegister("begin", 'b', new Option_Long(LONG_MIN));
    oc->doRegister("end", 'e', new Option_Long(LONG_MAX));
    // register Gawron's DUE-settings
    oc->doRegister("gBeta", new Option_Float(float(0.05)));
    oc->doRegister("gA", new Option_Float(1.0));
    // register the report options
    oc->doRegister("verbose", 'v', new Option_Bool(false));
    oc->doRegister("suppress-short-trip-warnings", new Option_Bool(false));
    oc->doRegister("print-options", 'p', new Option_Bool(false));
    oc->doRegister("help", new Option_Bool(false));
    // register the data processing options
    oc->doRegister("unsorted", new Option_Bool(false));
    oc->doRegister("save-cell-rindex", new Option_Bool(false));
    oc->doRegister("intel-cell", new Option_Bool(false));
    oc->doRegister("no-last-cell", new Option_Bool(false));
    oc->doRegister("use-lanes", 'l', new Option_Bool(false));
    oc->doRegister("scheme", 'x', new Option_String("speed"));
    oc->doRegister("no-sort", 'S', new Option_Bool(false));
    oc->doRegister("no-config", 'C', new Option_Bool(false));
    oc->addSynonyme("no-config", "no-configuration");
    if(OptionsIO::getOptions(oc, argc, argv)) {
        if(oc->getBool("help")) {
            HelpPrinter::print(help);
            delete oc;
            return 0;
        }
        if(oc->getBool("p"))
            cout << *oc;
        if(!checkSettings(oc)) {
            delete oc;
            throw ProcessError();
        }
    } else {
        delete oc;
        throw ProcessError();
    }
    return oc;
}


/**
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
RONet *
loadNet(ROLoader &loader, OptionsCont *oc)
{
    // load the net
    RONet *net = loader.loadNet();
    if(net==0)
        return 0;
    // load the weights when wished/available
    if(oc->isSet("w")) {
        loader.loadWeights(*net);
    }
    return net;
}


/**
 * Builds the output file
 * Informs about errors when occuring
 */
std::ofstream *
buildOutput(const std::string &name)
{
    std::ofstream *ret = new std::ofstream(name.c_str());
    if(!ret->good()) {
        SErrorHandler::add(
            string("The file '") + name +
            string("' could not be opened for writing."));
        throw ProcessError();
    }
    return ret;
}


/**
 * Computes the routes saving them
 */
void
startComputation(RONet &net, ROLoader &loader, OptionsCont &oc)
{
    // prepare the output
    std::ofstream *res =
        buildOutput(oc.getString("o"));
    std::ofstream *altres =
        buildOutput(oc.getString("o")+string(".alt"));
    // begin writing
    (*res) << "<routes>" << endl;
    (*altres) << "<route-alternatives>" << endl;
    // initialise the loader
    loader.openRoutes(net);
    // the routes are sorted - process stepwise
    if(!oc.getBool("unsorted")) {
        loader.processRoutesStepWise(
            oc.getLong("b"), oc.getLong("e"), *res, *altres, net);
    }
    // the routes are not sorted: load all and process
    else {
        loader.processAllRoutes(
            oc.getLong("b"), oc.getLong("e"), *res, *altres, net);
    }
    // end the processing
    loader.closeReading();
    // end writing
    (*res) << "</routes>" << endl;
    (*altres) << "</route-alternatives>" << endl;
    res->close();
    altres->close();
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
#ifdef _DEBUG
#ifdef WIN32
    CMemDiff state1;
#endif
#endif
    bool verbose = false;
    int ret = 0;
    RONet *net = 0;
    OptionsCont *oc = 0;
    try {
        // initialise the xml-subsystem
        //  (options and data loading)
        if(!XMLSubSys::init()) {
            return 1;
        }
        // get the options
        oc = getSettings(argc, argv);
        // test whether only the help was printed
        if(oc==0) {
            return 0;
        }
        // load data
        ROLoader loader(*oc);
        net = loadNet(loader, oc);
        if(net!=0) {
            // initialise the network for route computation
            net->postloadInit();
            // build routes
            try {
                startComputation(*net, loader, *oc);
            } catch (SAXParseException e) {
                cout << "Error:" << e.getLineNumber() << endl;
                ret = 1;
            } catch (SAXException e) {
                cout << "Error:"
                    << TplConvert<XMLCh>::_2str(e.getMessage())
                    << endl;
                ret = 1;
            }
        } else {
            ret = 1;
        }
        verbose = oc->getBool("v");
        delete oc;
    } catch (ProcessError) {
        cout << "Quitting (building failed)." << endl;
        ret = 1;
    }
    delete net;
    XMLSubSys::close();
    if(verbose&&ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}
