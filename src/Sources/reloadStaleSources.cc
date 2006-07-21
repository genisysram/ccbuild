/*
	ccbuild - A strict developer's build utility
	Copyright (C) 2005  A. Bram Neijt <bneijt@gmail.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "Sources.ih"

void Sources::reloadStaleSources()
{
	_foreach(src, d_sources)
	{
	 	if((*src)->stale())
	 	{
			if((*src)->reload())
				continue;

			//Reloading failed!
			_debugLevel1("Reload failed on " << (*src)->filename() << ". Cleaning up all sources.");

			cerr << "ccbuild: Source tree change. Forcing refresh.\n";

			//Forget all memory resident hashes
			MD5Info::destroy();

			//Remove the object of any binary target to ensure relinking
			_foreach(i, d_sources)
			{
				if((*i)->isBinTarget())
				{
					FileSystem::rmIfExists((*i)->outputFilename());
				}
			}
			
			//Cleanup
			//Remove all dependency knowledge because of stale pointers...
			_foreach(i, d_sources)
				delete *i;
			d_sources.clear();
			break;		
		}
	}
}
