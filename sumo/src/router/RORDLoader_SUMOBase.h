#ifndef RORDLoader_SUMOBase_h
#define RORDLoader_SUMOBase_h
//---------------------------------------------------------------------------//
//                        RORDLoader_SUMOBase.h -
//      The base class for SUMO-native route handlers
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.9  2006/01/26 08:37:24  dkrajzew
// removed warnings 4786
//
// Revision 1.8  2006/01/09 12:00:59  dkrajzew
// debugging vehicle color usage
//
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:50:40  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; trying to debug invalid vehicles handling
//
// Revision 1.3  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.2  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT;
//  preparation of classes to be derived for an online-routing
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// ------------------------------------------------
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include "ROTypedXMLRoutesLoader.h"
#include <utils/gfx/RGBColor.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/sumoxml/SUMOBaseRouteHandler.h>


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROVehicleType;
class RORouteDef;
class MsgHandler;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORDLoader_SUMOBase
 * As both the SUMO-routes and the SUMO-route alternatives handlers share
 *  a set of processing functions, we've joined these functions within a
 *  single class (this one).
 */
class RORDLoader_SUMOBase :
    public ROTypedXMLRoutesLoader, public SUMOBaseRouteHandler {
public:
    /// Constructor
    RORDLoader_SUMOBase(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin, SUMOTime end,
        const std::string &dataName, const std::string &file="");

    /// Destructor
    ~RORDLoader_SUMOBase();

    /** @brief Returns the name of the data
        "precomputed sumo route alternatives" is returned here */
    std::string getDataName() const;

    /// Returns the time the current (last read) route starts at
    SUMOTime getCurrentTimeStep() const;

protected:
    //{ XML-handling methods
    /** the user-impemlented handler method for an opening tag */
    virtual void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for a closing tag */
    virtual void myEndElement(int element, const std::string &name);
    //}

    /// Return the information whether a route was read
    bool nextRouteRead();

    /// Initialises the reading of a further route
    void beginNextRoute();

    /// Parses a vehicle type
    void startVehType(const Attributes &attrs);

    MsgHandler *getErrorHandlerMarkInvalid();

    /// begins the processing of a route
    virtual void startRoute(const Attributes &attrs) = 0;

    void closeVehicle();


protected:
    /// The type of the parsed file to allow a distinction
    std::string myDataName;

    /// The color of the current route
    RGBColor myCurrentColor;

    /** @brief Information whether the current route shall not be processed
        This may occure on errors */
    bool mySkipCurrent;

    /// Information whether a further route has been read
    bool myHaveNextRoute;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
