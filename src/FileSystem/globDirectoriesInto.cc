/*
   ccbuild - A strict developer's build utility

   Copyright (C) 2004 A. Bram Neijt <bneijt@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/


#include "FileSystem.ih"


namespace {

bool notDirectory(string target)
{
	//Return true if the string doesn't point to a normal file
	struct stat statbuf;
	if(stat(target.c_str(), &statbuf) == 0)
		if(S_ISDIR(statbuf.st_mode)) //Returns true if the statbuf denotes a regular file
			return false;
	return true;
}
}//anon namespace

void FileSystem::globDirectoriesInto(vector<string> *list, string const &pattern, bool sort)
{
	glob_t globbuf;	//Needs to be globfreed at the end

	//Use glob to get canditates
	//XXX GLOB_ONLYDIR doesn't seem to work well with simlinks
	if(sort)
		::glob(pattern.c_str(), GLOB_ONLYDIR | GLOB_TILDE, NULL, &globbuf);
	else
		::glob(pattern.c_str(), GLOB_ONLYDIR | GLOB_NOSORT | GLOB_TILDE, NULL, &globbuf);

	//Copy all the matches
	copy(&globbuf.gl_pathv[0],
			 &globbuf.gl_pathv[globbuf.gl_pathc],
			 back_inserter(*list)
			 );


	//Remove the not directory entries
	vector<string>::iterator endOfFiles = remove_if(list->begin(), list->end(), notDirectory);

	//Delete the elements
	list->erase(endOfFiles, list->end());

	//Cleanup
	globfree(&globbuf);
}
