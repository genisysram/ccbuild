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
void System::icmake()
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


    System::inspect(target);

    if(!target->isBinTarget())
      continue;

    System::icmake(target);
  }
}

void System::icmake(Source *source)
{
	System::inspect(source);

	vector<Source *> srcList;
	srcList.push_back(source);

	collectTargets(srcList);

	//Get the directories for all the files needed by this source
  set<string> dirList;
	__foreach(src, srcList)
		dirList.insert(FileSystem::baseName((*src)->directory()));

	//Never include the local directory "."
	if(dirList.count(".") != 0)
		dirList.erase(".");

	coutLock.set();
	copy(dirList.begin(), dirList.end(), ostream_iterator<string>(cout, "\n"));
	coutLock.unset();
}

