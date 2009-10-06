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



#include "system.ih"
void System::check()
{
  vector<string> files;

  Sources &s = Sources::getInstance();

  FileSystem::globSourceFilesInto(&files, ".");

  __foreach(file, files)
  {
    Source *target = s[*file];

    //Error loading source??
    if(target == 0)
    {
      cerr << "Error loading '" << *file << "'\n";
      continue;
    }

    System::inspect(target);

    //isBinTarget test can only be done after the inspect
    if(!target->isBinTarget())
      continue;
		cerr << "[check " << target->filename() << "]\n";
    System::check(target);
  }
}


void System::check(Source *source)
{
	
  vector<Source *> srcList;
  srcList.push_back(source);
  collectTargets(srcList);
  
	if(Options::xml)
	{
		cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		cout << "<!-- Generated by ccbuild " << Options::version << ", see " << Options::homepage << " -->\n\n";
		cout << "<buildstates>\n";
  }
	__foreach(src, srcList)
	{
		if(Options::xml)
			cout << "  <state value=\"";

		cout << ((*src)->upToDate() ? "ok" : "old");
		
		if(Options::xml)
			cout << "\" ";
		
		if(Options::verbose)
		{
			if(Options::xml)
				cout << "type=\"";
			else
				cout << "\t";
			
			if((*src)->isHeader())
			{
				if((*src)->isInternalHeader())
					cout << "ih";
				else
					cout << "hh";
			}
			else if((*src)->isBinTarget())
				cout << "bt";
			else
				cout << "ot";
			
			if(Options::xml)
				cout << "\" ";
		}
		if(Options::xml)
			cout << "src=\"";
		else
			cout << "\t";
			
		cout << (*src)->filename();
		
		if(Options::xml)
			cout << "\" />";

		cout << "\n";
		
	}
	
	if(Options::xml)
		cout << "</buildstates>\n";
}
