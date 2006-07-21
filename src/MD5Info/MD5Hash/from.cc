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

#include "MD5Hash.ih"



void MD5Hash::from(std::string const &filename)
{
	d_loaded = true;
//	if(Options::option<bool>("verbose"))
//		cerr << "[MD5] " << filename << "\n";
	ifstream file(filename.c_str());
	MD5 algo(file);
	unsigned char *digest = algo.raw_digest();
	for(unsigned i = 0; i < d_size; ++i)
		d_hash[i] = digest[i];
	delete[] digest;
}
