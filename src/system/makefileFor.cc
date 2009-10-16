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

void System::makefileFor(Source *root, ostream &str)
{
	if(!Options::makefileHeaderDone)
	{
		Options::makefileHeaderDone = true;
		
		str << "########\n";
		str << "#Makefile generated by ccbuild version "<< Options::version << "\n";
		str << "#For more information visit: " << Options::homepage << "\n";
		str << "########\n\n\n";

		str << "#Directory rule\n";
		str << "o/:\n\tmkdir $@\n\n";
		str << "%/o/:\n\tmkdir $@\n\n";

		str << "CC := " << Options::CC << "\n";
		str << "CCADDFLAGS := " << Options::extraArgs << "\n\n";
		Options::extraArgs = "";
		
		str << ".PHONY: distclean\n";
		str << "distclean:\n";
		str << "\trm -rf o;\n";
		//str << "\tfind ./ -type f -path \"*/o/*.md5\" -exec \"rm\" \"-f\" \"{}\" \\;\n";
		//str << "\tfind ./ -type f -name \"*.gch\" -exec \"rm\" \"-f\" \"{}\" \\;\n";
		//str << "\tfind ./ -depth -type d -name \"o\" -exec \"rmdir\" \"{}\" \\;\n";
		str << "\n";
	}

	vector<Source *> targets;
	targets.push_back(root);
	collectTargets(targets);
	
	Compiler cc("$(CC) $(CCADDFLAGS)");

	Resolver &resolver = Resolver::getInstance();

	str << "########\n";
	str << "# Starting section for: " << root->filename() << "\n";
	str << "########\n\n";
	
	vector<string> dirs;
	
	//All the object rules..
	__foreach(src, targets)
	{
		if((*src)->isObjectTarget())
		{
			dirs.push_back(FileSystem::directoryName((*src)->outputFilename()));
			
			str << (*src)->outputFilename() << ": " << (*src)->filename();
			vector<string *> global;
			vector<Source *> local;
			(*src)->dependencies(local, global); //Objects depend on only headers...
			__foreach(l, local)
			{
				str << " " << (*l)->filename();
				if((*l)->isInternalHeader() && Options::precompile)
					str << " " << (*l)->filename() << ".gch";
				else if((*l)->isHeader() && Options::precompileAll)
					str << " " << (*l)->filename() << ".gch";
			}
			str << "\n";

			//Fill the compiler.
			__foreach(gl, global)
				resolver.resolveInto(*gl, cc);

			str << "\t" << cc.compileCommand(
													"",
													(*src)->filename(),
													(*src)->outputFilename()
													)
									 << "\n";
			cc.addObject((*src)->outputFilename());
			//Empty the compiler
			cc.rmCompileOptions();
			str << "\n";
		}
		else if((*src)->isInternalHeader() && Options::precompile)
		{
			str << (*src)->outputFilename() << ": " <<  (*src)->filename();
			vector<string *> global;
			vector<Source *> local;
			(*src)->dependencies(local, global); //Objects depend on only headers...
			__foreach(l, local)
					str << " " << (*l)->filename();
			str << "\n";

			//Fill the compiler.
			__foreach(gl, global)
				resolver.resolveInto(*gl, cc);

			str << "\t" << cc.precompileCommand(
													"",
													(*src)->filename(),
													(*src)->outputFilename()
													)
									 << "\n";
									 
			//Empty the compiler
			cc.rmCompileOptions();
			str << "\n";			
		}
		else if((*src)->isHeader() && Options::precompileAll)
		{
			str << (*src)->outputFilename() << ": " <<  (*src)->filename();
			vector<string *> global;
			vector<Source *> local;
			(*src)->dependencies(local, global); //Objects depend on only headers...
			__foreach(l, local)
					str << " " << (*l)->filename() << "";
			str << "\n";

			//Fill the compiler.
			__foreach(gl, global)
			{
				resolver.resolveInto(*gl, cc);
			}
			str << "\t" << cc.precompileCommand(
													"",
													(*src)->filename(),
													(*src)->outputFilename()
													)
									 << "\n";
			//Empty the compiler
			cc.rmCompileOptions();
			str << "\n";			
		}
	}


	str << root->outputFilename() << ": " << root->directory() + "/o/ " <<  root->filename();
	
	vector<Source *> deps;
	vector<string *> global;
	root->dependencies(deps, global);
	__foreach(l, deps)
	{
		if((*l)->isInternalHeader() && Options::precompile)
			str << " " << (*l)->filename() << ".gch";
		else if((*l)->isHeader() && Options::precompileAll)
			str << " " << (*l)->filename() << ".gch";
		str << " " << (*l)->filename();
	}
	str << "\n";

	//Fill the compiler.
	__foreach(gl, global)
		resolver.resolveInto(*gl, cc);

	str << "\t" << cc.compileCommand(
											"",
											root->filename(),
											root->outputFilename()
											)
							 << "\n";
	cc.addObject(root->outputFilename());

	//Empty the compiler
	cc.rmCompileOptions();
	str << "\n";
	
	//Rule for binaryTarget object directory
	dirs.push_back(root->directory() + "/o/");
	
	//The root rule: Depend on all objects
	str << root->directory() << "/" << root->basename() << ": ";

	//Depend on directories
	sort(dirs.begin(), dirs.end());
	unique_copy(dirs.begin(), dirs.end(), ostream_iterator<string>(str, " "));

	//Depend on objects
	vector<string> const &objects = cc.objects();
	__foreach(obj, objects)
		str << " " << *obj;

	str << "\n";

	//Fill the compiler.
	__foreach(gl, global)
		resolver.resolveInto(*gl, cc);

	str << "\t" << cc.linkCommand(
											root->directory(),
											root->basename()
											)
							 << "\n\n";

}