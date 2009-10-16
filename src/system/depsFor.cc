/*
  This file is part of ccbuild.

  ccbuild is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ccbuild is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ccbuild.  If not, see <http://www.gnu.org/licenses/>.
*/






#include "system.ih"

void System::depsFor(ostream &str)
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
      cout << "Error loading '" << *file << "'\n";
      continue;
    }

    System::depsFor(target, str);
  }
}

void System::depsFor(Source * s, ostream &str)
{
	vector<Source *> srcList;
	vector<string *> globalList;
	vector<string > ignored;

	System::inspect(s);
	s->dependencies(srcList, globalList);
	s->ignoredDeps(ignored);


	__foreach(src, srcList)
		str << (*src)->filename() << " ";
  str << "\n\n";
	copy(ignored.begin(), ignored.end(), ostream_iterator<string>(str, " "));
  str << "\n\n";

	__foreach(src, globalList)
		str << (**src) << " ";
	str << "\n";
}