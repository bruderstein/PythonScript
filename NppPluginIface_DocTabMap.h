/* NppPluginIface_DocTabMap.h
 *
 * This file is part of the Notepad++ Plugin Interface Lib.
 * Copyright 2008 - 2009 Thell Fowler (thell@almostautomated.com)
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program;
 * if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 *  Notepad++ Plugin Interface Lib extension providing mapping of Notepad++ tab bufferID and
 *  visibility to Scintilla Document pointers, as well as tracking the currently open files
 *  by docID and a docId to bufferID map.
 *
 *  For an example of using this see NppPluginIface_ActionIndex.
 *
 */


#ifndef NPP_PLUGININTERFACE_DOCTABMAP_EXTENSION_H
#define NPP_PLUGININTERFACE_DOCTABMAP_EXTENSION_H

#include "NppPluginIface.h"

namespace npp_plugin {

namespace doctabmap {


void update_DocTabMap();
int getVisibleDocId_by_View( int view );
int getDocIdFromBufferId( int bufferID );
bool fileIsOpen( int pDoc );


} // End namespace: doctabmap

} // End namespace: npp_plugin

#endif //  End include guard: NPP_PLUGININTERFACE_ACTIONINDEX_EXTENSION_H
