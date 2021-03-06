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


namespace {
//Function object returning either "" or the \t<option>\n if the given option was found.
class printArguments {
        string d_start;
    public:
        printArguments(string const &beginning)
            :
            d_start(beginning)
        {}

        std::string operator()(std::string const &rvalue) const {
            if(startsWith(rvalue, d_start)) {
                return "\t\"" + rvalue + "\"\n";
            }
            return "";
        }

    private:
        bool startsWith(std::string const &a, std::string const &b) const {
            if(a.size() < b.size()) {
                return false;
            }
            return a.compare(0, b.length(), b) == 0;
        }
};


}//Anon namespace

//TODO Cleanup indentation and segmentation for this code, it's becoming to large
void System::aapFor(Source *root, ostream &str) {

    if(!Options::aapHeaderDone) {
        Options::aapHeaderDone = true;
        str << "########\n";
        str << "# A-A-P rules generated by ccbuild version "<< Options::version << "\n";
        str << "# For more information visit: " << Options::homepage << "\n";
        str << "#\n";
        str << "# To add extra build information, use Extra(Libs|Include|Define)For<Program basename>\n";
        str << "# For example: ExtraLibsForCcbuild = \"-L/home -lmyhome\"\n";
        str << "########\n\n\n";


        //Include directories
        vector<string> const &incPaths = Options::includePaths;
        if(incPaths.size() > 0) {
            str << "# Default includes from ccbuild call\n";
            str << "CcbuildDefaultIncludes += \\\n";
            __foreach(p, incPaths)
            str << "\t-I\"" << *p << "\"\n";
            str << "\n";
        }

        //Optimize options
        /*
        if(Options::option<std::string>("extraArgs").find_first_of("-O0") != string::npos)
            str << "OPTIMIZE = 0\n";
        else if(Options::option<std::string>("extraArgs").find_first_of("-O1") != string::npos)
        {}
        else if(Options::option<std::string>("extraArgs").find_first_of("-O2") != string::npos)
        {}
        else if(Options::option<std::string>("extraArgs").find_first_of("-O3") != string::npos)
            str << "OPTIMIZE = 9\n";
        */
    }

    vector<Source *> targets;
    targets.push_back(root);
    collectTargets(targets);

    //Collect all libraries needed:
    Resolver &resolver = Resolver::getInstance();
    set<string> globals;
    Compiler cc;
    __foreach(src, targets) {
        vector<string const*> global;
        vector<Source *> local;
        (*src)->dependencies(local, global);

        //Fill the compiler.
        __foreach(gl, global)
        resolver.resolveInto(*gl, cc);

        //Store a global set
        __foreach(gl, global)
        globals.insert(**gl);
    }

    //Argument parsing and listing
    vector<string> arguments;
    vector<string>::iterator it;
    string rootIdent = root->basenameWithoutExtension();
    rootIdent[0] = toupper(rootIdent[0]);

    //Compile arguments
    vector<string> cppArgs = cc.compileArguments();

    //Copy extra (commandline given) arguments
    istringstream extraOpts(Options::extraArgs);
    copy(std::istream_iterator<std::string>(extraOpts), std::istream_iterator<std::string>(), back_inserter(cppArgs));

    //Print the include arguments
    transform(cppArgs.begin(), cppArgs.end(), back_inserter(arguments), printArguments("-I"));
    it = remove(arguments.begin(), arguments.end(), "");  //Remove empty elements
    arguments.erase(it, arguments.end());
    if(!arguments.empty()) {
        str << "# Include paths for " << root->filename() << "\n";
        str << "GeneratedIncludeFor" << rootIdent << " = \\\n";
        copy(arguments.begin(), arguments.end(), ostream_iterator<string>(str));
        arguments.clear();
        str << "\n";
    }
    str << "IncludeFor" << rootIdent << " = $?CcbuildDefaultIncludes $?GeneratedIncludeFor" << rootIdent << " $?ExtraIncludeFor" << rootIdent << "\n\n";


    //Print the define arguments
    //TODO turn into a copy_if
    transform(cppArgs.begin(), cppArgs.end(), back_inserter(arguments), printArguments("-D"));
    it = remove(arguments.begin(), arguments.end(), "");  //Remove empty elements
    arguments.erase(it, arguments.end());
    if(!arguments.empty()) {
        str << "# Defines for " << root->filename() << "\n";
        str << "GeneratedDefineFor" << rootIdent << " = \\\n";
        copy(arguments.begin(), arguments.end(), ostream_iterator<string>(str));
        arguments.clear();
        str << "\n";
    }
    str << "DefineFor" << rootIdent << " = $?GeneratedDefineFor" << rootIdent << " $?ExtraDefineFor" << rootIdent << "\n\n";


    //Link arguments
    //Print the library arguments
    vector<string> const &linkArgs = cc.linkArguments();
    //Library path additions
    transform(linkArgs.begin(), linkArgs.end(), back_inserter(arguments), printArguments("-L"));
    transform(linkArgs.begin(), linkArgs.end(), back_inserter(arguments), printArguments("-l"));
    it = remove(arguments.begin(), arguments.end(), "");  //Remove empty elements
    arguments.erase(it, arguments.end());
    if(!arguments.empty()) {
        str << "# Library flags for " << root->filename() << "\n";
        str << "GeneratedLibsFor" << rootIdent << " = \\\n";
        copy(arguments.begin(), arguments.end(), ostream_iterator<string>(str));
        arguments.clear();
        str << "\n";
    }
    str << "LibsFor" << rootIdent << " = $?GeneratedLibsFor" << rootIdent << " $?ExtraIncludeFor" << rootIdent << "\n";


    //Conf rules for all headers
    if(globals.size() > 0) {
        str << "# Sorted list of used global headers\n";
        str << "GlobalHeadersFor" << rootIdent << " = \\\n";
        __foreach(global, globals)
        str << "\t\"" << *global << "\"\n";//Optional if :conf lang is dropped use {language = C++}\n";
        str << "\n";

        str << "# Configuration generation (use by including config/basename.hh in your program)\n"
            << "ConfigurationFilenameFor" << rootIdent << " ?= " << root->directory() << "/config/" << root->basenameWithoutExtension() << ".hh\n"
            << "$ConfigurationFilenameFor" << rootIdent << " :\n"
            << "\t:conf init\n"
            << "\t_conf.INCLUDE += $IncludeFor" << rootIdent << "\n"
            << "\t:conf language C++\n"
            << "\t:conf header {required} $GlobalHeadersFor" << rootIdent << "\n"
            << "\t:touch {exist} `os.path.dirname(_no.ConfigurationFilenameFor" << rootIdent << ")` {directory}\n"
            << "\t:conf write header $ConfigurationFilenameFor" << rootIdent << "\n"
            << "CLEANFILES += $ConfigurationFilenameFor" << rootIdent << "\n"
            << "\n";
    }


    //Main program rule
    str << ":program " << root->directory() << "/" << root->basenameWithoutExtension() << "\n";
    str << "\t{ comment = \"ccbuild "
        << Options::version
        << " generated program recipe for " << root->filename() << "\"}\n";
    str << "\t{ var_LIBS = $LibsFor" << rootIdent << "}\n";
    str << "\t\t: \n";
    str << "\t{ var_INCLUDE = $IncludeFor" << rootIdent << "}\n";
    str << "\t{ var_DEFINE = $DefineFor" << rootIdent << "}\n";
    //Already in objectTargets str << "\t\"" << root->filename() << "\"";
    __foreach(src, targets) {
        if((*src)->isObjectTarget()) {
            str << "\n\t\"" << (*src)->filename() << "\"";
        }
    }
    str << "\n\n";
}

