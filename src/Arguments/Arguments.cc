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
#include "Arguments.ih"

Arguments::Arguments(Option const *options, int argc, char *argv[])
	:
	d_empty(""),
	d_options(options)
{
	//Reset all getOpts extern variables
  optarg = 0;
  optind = 0;
  opterr = 0;
  optopt = 0;

	//For the getopts_long options system we need:
	// A string of single character options, and wether they take arguments
	//
	//
	ostringstream letterFlags(ios::app);


	//TODO In the future we will count the number of long options before hand, so dynamically allocate this array.
	struct ::option *longOpts = new ::option[50];
	unsigned nlongOpts(0);

	//Collect options
	int optionIndex(0);	//Although this should be an unsigned, get_longOpts wants an int :-(
	while(true)
	{
		Option const * curOption = &options[optionIndex];
		bool hasArgument = false, mustHaveArgument = false;


		//Done yet??
		if(curOption->flags.empty() && curOption->value.empty() && curOption->helpText.empty())
			break;


		istringstream givenFlags(string(curOption->flags) + " ");	
		//^^ Needed extra space for eof in while loop below

		if(curOption->value.length() > 0 && curOption->value[0] != '\0')
		{
			mustHaveArgument = curOption->value[0] == '<';
			hasArgument = curOption->value[0] == '[' || mustHaveArgument;
		}

		//Parse options..
		string flag;
		bool *flagPointer = new bool(false);

		while(true)
		{
			givenFlags >> flag;
			// cout << "givenFlags: " << givenFlags.str() << "\n";

			if(givenFlags.eof())
				break;


			if(flag.size() == 1)
			{
				d_flags[flag] = flagPointer;
				letterFlags << flag;
				if(hasArgument)
				{
					letterFlags << ":";
					if(!mustHaveArgument)
						letterFlags << ":";
				}
			}
			else
			{



				//Name
				longOpts[nlongOpts].name = new char[flag.size() +1];
				strncpy(const_cast<char *>(longOpts[nlongOpts].name),
								flag.c_str(),
								flag.size() +1);

				//What kind of argument is it?
				longOpts[nlongOpts].has_arg = hasArgument ? optional_argument : no_argument;
				if(mustHaveArgument)
					longOpts[nlongOpts].has_arg = required_argument;

				//Return val on match
				longOpts[nlongOpts].flag = 0;

				//Give the index of the option +255 as a return value, it will always be above ascii
				//longOpts[nlongOpts]->val = 255 + nlongOpts;
				longOpts[nlongOpts].val = 0;	//if it's a long option, the index is enough.

				//Store the flag pointer value
				d_flags[flag] = flagPointer;

				++nlongOpts;
			}
		}
		++optionIndex;
	}

	//Add empty element, superflourisch if newly allocated space contains zeros
	longOpts[nlongOpts].name = 0;
	longOpts[nlongOpts].has_arg = 0;
	longOpts[nlongOpts].flag = 0;
	longOpts[nlongOpts].val = 0;

	//++nlongOpts;
	#if DEBUGLEVEL > 3
	_debugLevel3("Flags " << letterFlags.str());
	_debugLevel3("nlongOpts " << nlongOpts);

	for(unsigned i=0; i < nlongOpts; ++i)
	{
		cerr << i << ")\tName " << longOpts[i].name << "\n";
		cerr << "\tHasArg " << longOpts[i].has_arg << "\n";
		cerr << "\tVal " << longOpts[i].val << "\n";
		cerr << "\tFlag " << longOpts[i].flag << "\n";
		cerr << "\n";
	}
	_debugLevel3("Shortopts: \"" << letterFlags.str() << "\"");
	#endif
	//
	// Run the options through getopt_long and collect them in vectors
	//
	::opterr = 0;	//Don't report errors on stderr

	//The extra byte below this is just a precaution
	// for systems where the sizeof(char) == sizeof(int) :-S
	char *flagVal = new char[sizeof(int) +1];
	string optionCharacter(" ");
	while(true)
	{

		int c = getopt_long(argc, argv, letterFlags.str().c_str(),
								longOpts, &optionIndex);
		if(c == -1)
			break;

		bool *flagPointer;
		switch(c)
		{
			case 0:
				//Long options
			  flagPointer = d_flags[longOpts[optionIndex].name];
			  *flagPointer = true;

				if(::optarg)
					d_values[flagPointer].push_back(::optarg);

  	    break;
			case '?':
			  _debugLevel4("Case ?");
			  _debugLevel4("optarg: " << (optarg ? optarg : ""));
				_debugLevel4("optind: " << optind << " optopt: " << optopt);
				if(optopt || optind < 1 || optind > argc)
				{
					optionCharacter[0] = optopt;
					d_errors.push_back(optionCharacter);
				}
				else
					d_errors.push_back(argv[optind -1]);
					
				break;

			case ':':
			  _debugLevel4("Case :");
			  _debugLevel4("optarg: " << optarg);
				_debugLevel4("optind: " << optind << " optopt: " << optopt);
				optionCharacter[0] = optopt;
				d_errors.push_back(optionCharacter);
				break;

			default:
				//Short options
				flagVal[0] = static_cast<char>(c);
				flagVal[1] = '\0';
				
				_debugLevel2("OK on short option: '" << flagVal << "'");

				flagPointer = d_flags[flagVal];
				*flagPointer = true;

				if(::optarg)
					d_values[flagPointer].push_back(::optarg);
		}
	}
	delete[] flagVal;

	//Get the rest of the options
	for(; ::optind < argc; ++(::optind))
		d_rest.push_back(argv[::optind]);


	//Test code
/* cout << "Arguments done\n";
	_foreach(s, d_rest)
		cout << "\t" << *s << "\n";
*/

	//Free the memory
	for(unsigned i = 0; i < nlongOpts; ++i)
		if(longOpts[i].name)
			delete[] longOpts[i].name;
	delete[] longOpts;

}


Arguments::~Arguments()
{
	//Free by deleting all boolean pointers
	//Reduce memory usage
	clear();
}
