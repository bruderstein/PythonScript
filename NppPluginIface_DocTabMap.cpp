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


#include "NppPluginIface_DocTabMap.h"

#include <set>
#include <map>

//  <--- Notepad++ Scintilla Components for BufferID to pDoc --->
#define TIXMLA_USE_STL
#include "Buffer.h"

//  <--- Boost --->
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>


namespace npp_plugin {


namespace doctabmap {

//  The main class for storing the document to buffer state information.
struct DocTab {
	HWND _view;
	int _tab;
	BufferID _bufferID;
	Document _pDoc;
	bool _isVisible;

	DocTab( HWND view, int tab, BufferID bufferID, Document pDoc, bool isVisible)
		:_view(view), _tab(tab), _bufferID(bufferID), _pDoc(pDoc), _isVisible(isVisible){};
};


//  Boost mulit_index_container setup for tracking tab visibility states.
using boost::multi_index::multi_index_container;
using namespace boost::multi_index;

//  Namespace for the DocTab multi_index_container.
namespace doctabmap_mic {

//  DocTab_set index key for comp_visible_key.
struct dt_visible_key{};

//  DocTab_set composite index key for unique view/visible combination.
struct comp_visible_key:composite_key<
	DocTab,
	BOOST_MULTI_INDEX_MEMBER(DocTab, HWND, _view),
	BOOST_MULTI_INDEX_MEMBER(DocTab, bool, _isVisible)
>{};

//  Multi Index Container for indexing N++ Doc Tab state data.
typedef multi_index_container<
	DocTab,
	indexed_by<
		ordered_unique< tag<dt_visible_key>, comp_visible_key >
	>
> DocTab_set;

//  Index definition type for the DocTab_set container.
typedef DocTab_set::index<dt_visible_key>::type dts_by_visibleTab;

//  The one and only DocTab_set container.
DocTab_set dts;

//  A set for storing active and ready open documents
std::set<int> open_docs;

//  A map for storing buffer id to doc id.  Usefull for doc open/close operations.
std::map< int, int > buff2doc_map;

} // End namespace doctabmap_mic

using namespace doctabmap_mic;

//  Public index to the DocTab_set.
dts_by_visibleTab& visibleTab_index = dts.get<dt_visible_key>();

//  Update the tab mapping between bufferIDs and pDocs for message filtering.
void update_DocTabMap()
{
	//  Since there isn't a buffer-deactivate message to remove an entry we empty the set and
	//  rebuild on buffer activated messages.
	if ( dts.size() > 0 ) {
		dts.clear();
		open_docs.clear();
	}

	//  Populate the DocTab_set
	//  The tab and buffer ID aren't really needed, yet I believe there may be some other
	//  uses for that information so they've been left in there.
	bool isVisible = false;
	for ( int view = MAIN_VIEW; view <= SUB_VIEW; view++ ) {
		HWND targetView = hViewByInt(view);
		Document viewDocID = (Document)::SendMessage( targetView, SCI_GETDOCPOINTER, 0, 0);
		int nb_openfiles_view = ( view == MAIN_VIEW ) ? ( PRIMARY_VIEW ) : ( SECOND_VIEW );
		int nb_Tabs = ::SendMessage( hNpp(), NPPM_GETNBOPENFILES, 0, nb_openfiles_view );
		for ( int tab = 0; tab < nb_Tabs; tab++ ) {
			int tabBuffID = ::SendMessage( hNpp(), NPPM_GETBUFFERIDFROMPOS, tab, view);
			if ( tabBuffID > 0 ) {
				BufferID pBuff = (BufferID)tabBuffID;;
				Document pDoc = pBuff->getDocument();
				isVisible = ( pDoc == viewDocID ) ? ( true ) : ( false );
				dts.insert(
					DocTab( targetView, tab, pBuff, pDoc, isVisible)
				);
				open_docs.insert( (int)pDoc );
				if ( buff2doc_map.find( (int)pBuff ) == buff2doc_map.end() ) {
					buff2doc_map[ (int)pBuff ] = (int)pDoc;
				}
			}
		}
	}
}

//  Returns the Scintilla Document ID for the visible document in the specified view.
//  ( MAIN_VIEW = 0, SUB_VIEW = 1 )
int getVisibleDocId_by_View( int view )
{
	//  The tuple is composed of the HWND for the view and a bool for 'is visible' to lookup
	//  the Doc ID based on the composite key from the index.
	DocTab _DocTab = *(visibleTab_index.find( boost::make_tuple( hViewByInt( view ), true ) ) );

	return ( _DocTab._pDoc );
}

//  Returns the Scintilla Document ID matching a buffer ID.
int getDocIdFromBufferId( int bufferID ) { return ( buff2doc_map[bufferID] ); }

//  Returns true if a document is currently open and ready.
bool fileIsOpen( int pDoc )
{
	if ( open_docs.find( pDoc ) != open_docs.end() ) return ( true );
	else return ( false );
}

} // End namespace: doctabmap

} // End namespace: npp_plugin
