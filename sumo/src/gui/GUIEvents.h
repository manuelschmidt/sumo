#ifndef GUIEvents_h
#define GUIEvents_h
//---------------------------------------------------------------------------//
//                        GUIEvents.h -
//  An enumeration of SUMO-Events
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
// Revision 1.4  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on end and be started with a simulation now;
//
// Revision 1.3  2003/06/18 11:04:53  dkrajzew
// new error processing adapted
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qevent.h>


/* =========================================================================
 * our own event enumeration
 * ======================================================================= */
/**
 * As events are distinguished by their number, here is the enumeration
 * of our custom events
 */
enum GUIEvent {
    /// send when a simulation has been loaded
    EVENT_SIMULATION_LOADED,

    /// send when a simulation step has been performed
    EVENT_SIMULATION_STEP,

    /// send when a message occured
    EVENT_MESSAGE_OCCURED,

    /// send when a warning occured
    EVENT_WARNING_OCCURED,

    /// send when a error occured
    EVENT_ERROR_OCCURED,

    /** @brief Send when the simulation is over;
        The reason and the time step are stored within the event */
    EVENT_SIMULATION_ENDED

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIEvents.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

