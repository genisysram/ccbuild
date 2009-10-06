/*
  ccbuild - A strict developer's build utility
  Copyright (C) 2008  A. Bram Neijt <bneijt@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



#include "Source.ih"
void Source::buildHeader(Compiler & cc)
{
	//To build a header, precompile it


  vector<Source *> srcList;
  vector<string *> globalList;
	dependencies(srcList, globalList);

	Resolver &resolver = Resolver::getInstance();

  //Resolve all globals into the compiler
  __foreach(global, globalList)
  	resolver.resolveInto(*global, cc);

	if(!upToDate(srcList))
	{
    int ret = cc.precompile(directory(), d_filename, outputFilename());
    
    //Compilation OK
		//Update hash
		if(Options::md5 && ret == 0)
		{
			MD5Info &md5i = MD5Info::getInstance();
			string collectedHash = md5i.contentHash(filename());
			
			vector<Source *> srcList;
			dependencies(srcList);
			__foreach(src, srcList)
				collectedHash += md5i.contentHash((*src)->filename());
			
			md5i.save(filename(), collectedHash);
		}
	}
}
