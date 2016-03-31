isxSQLite
=========
isxSQLite is an extension for [InnerSpace](http://www.lavishsoft.com/).   Although the primary purpose of isxSQLite is to provide the tools necessary for InnerSpace users to incorporate SQLite 3 functionality within Lavishscript, it is also being released as open source in order to provide the public with a working example of an InnerSpace extension.  

It is the goal of this project to provide the public with source code examples of all things possible with an InnerSpace extension.  Please feel free to create an "issue" here on github if you have any requests or questions that might be answered with sample code (additional functionality) within isxSQLite.



Building isxSQLite.dll
--------------------
isxSQLite is designed for no-hassle compiling using Visual Studio 2015.  All required libraries and headers are included here on the repository.

1.  Double-click `isxSQLite.sln` to load the solution in Visual Studio 2015.
2.  Ensure that the build configuration is set to 'Release', which is the default, or 'Release (x64)', if you prefer a x64 extension. 
3.  Right-click on the isxSQLite project in the Solution Explorer and select "Properties".  Set the "Output Directory" to a valid location for the current configuration.   For 'release', you would want it to be your /innerspace/Extensions/ISXDK35 folder.   For 'release (x64)', it should be /innerspace/x64/Extensions/ISXDK35.

At this point, simply choose "Build->Build Solution" and the isxSQLite.dll will be generated.

### Technical Notes
1.  The files within the `libisxgames` directory contain code taken from the library used by Amadeus for all extensions released at isxGames.com.   You will notice preprocessor directives throughout the solution instructing the compiler when to use code from this extraction, and when to use the actual library (i.e., when Amadeus builds a binary for official release.)   Users may use code from libisxgames if they wish (it's released under the same license as the rest of isxSQLite); however, most users should simply ignore any reference to `libisxgames` found within the isxSQLite source code.



Using isxSQLite with InnerSpace
-------------------------------
Assuming that isxSQLite.dll is located in the correct Extensions folder (see step 3 above), you can load the extension by typing `ext isxSQLite` inside the InnerSpace console.  

There is an example script located in the `+Scripts+` repository folder.   You may copy this script to your `/innerspace/Scripts` folder and then edit it in order to test isxSQLite functionality.   The script is heavily commented, so users are encouraged to read through it before asking questions.



Documentation
-------------
The source code is designed to be the primary documentation for isxSQLite; however, users are encouraged to read through `isxSQLiteChanges.txt` and `+Scripts+\sqlite.iss`.  These files will be updated if/when new features are added to the extension.



Copyright
---------
isxSQLite is Copyright 2011-2016 by [Amadeus@isxGames.com](http://www.isxgames.com/).  Permission to use the source code in this project is granted under the Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit http://creativecommons.org/licenses/by/3.0/ for a summary of what rights are granted under this license.