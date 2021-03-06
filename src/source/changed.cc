/*
  This file is part of ccbuild.
  Copyright (C) 2013  A. Bram Neijt <bneijt@gmail.com>

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
#include "source.ih"
bool Source::changed()  const {
    //Without output, we can't check. So we probably are old
    if(producesOutput() && !FileSystem::fileExists(outputFilename())) {
        _debugLevel1("File changed because it produces output and the output does not exist\n"
                     << "  filename: " << outputFilename()
                    );
        return true;
    }
    //MD5 check
    if(Options::md5) {
        //This file has changed if the first 32 chars of it's contentHash are
        // not the same as the first 32 chars of it's stored hash.
        MD5Info &md5i = MD5Info::getInstance();
        return md5i.old(filename()).compare(0, 32, md5i.contentHash(filename())) != 0;
    }

    return FileSystem::newer(filename(), outputFilename());
}
bool Source::changed(std::string const &relativeToThis) const {
    //Relative changes are only checkable with timestamps currently
    assert(Options::md5 == false);

    //Wether we create output or not, we can check our status
    //According to this "relativeToThis" output.
    return FileSystem::newer(filename(), relativeToThis);
}
