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
#include "compiler.ih"

string Compiler::precompileCommand(std::string target,
                                   std::string outputFile) const {
    ostringstream command(d_baseCommand, ios::ate);

    command << " " << Options::extraArgs << " ";

    copy(d_compile.begin(), d_compile.end(), ostream_iterator<string>(command, " "));

    command << "-c -o \"" << outputFile << "\" -x \"c++-header\" \"" << target << "\" ";
    command << Options::commandAppend;
    return command.str();
}
