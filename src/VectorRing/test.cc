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

#include "VectorRing.hh"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <typeinfo>

using namespace std;
using namespace bneijt;
int main(int argc, char **argv)
try
{
  VectorRing<string> v(3);
  string a("a");
  for(char i = 'a'; i < 'z'; ++i)
  {
    cout << "=\n";
    v.push_back(a);
    copy(v.begin(), v.end(), ostream_iterator<string>(cout));
    a[0] = i;
  }
  cout << "\n";
  return 0;
}
catch(const std::exception &e)
{
	//All is LOST... nothing to do here but die
  cerr << "Caught std::exception (" << typeid(e).name() << "): " << e.what();
  cerr << "\nPlease report this as a bug.\n";
}