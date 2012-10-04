ISXSQLite
=========
ISXSQLite is an extension for [InnerSpace](http://www.lavishsoft.com/)   The primary purpose of ISXSQLite is to provide the tools necessary for InnerSpace users to incorporate SQLite 3 functionality within Lavishscript, it is also being released as open source in order to provide the public with a working example of an InnerSpace extension.  Currently, the extension supports Lavishscript 1.0; however, it will be updated to additionally support Lavishscript 2.0 once it is released.

It is the goal of this project to provide the public with source code examples of all things possible with an InnerSpace extension.  Please feel free to create an "issue" here on github if you have any requests or questions that might be answered with sample code (additional functionality) within ISXSQLite.



Building ISXSQLite.dll
--------------------
ISXSQLite is designed for no-hassle compiling using Visual Studio 2010.  All required libraries and headers are included here on the repository.

1.  Double-click `ISXSQLite.sln` to load the solution in Visual Studio 2010.
2.  Ensure that the build configuration is set to 'Release' (default)
3.  Right-click on the ISXSQLite project in the Solution Explorer and select "Properties".  Ensure that the "Output Directory" is set to a valid location.  You will most likely want it to be your `/InnerSpace/Extensions/` directory.   (The default is `..\Extensions\', which assumes that your isxSQLite directory is at `/InnerSpace/ISXSQLite`.)

At this point, simply choose "Build->Build Solution" and the ISXSQLite.dll will be generated.



Using ISXSQLite with InnerSpace
-------------------------------
Assuming that ISXSQLite.dll is located in your `/innerspace/Extensions` folder, you can load the extension by typing `ext ISXSQLite` inside the InnerSpace console.  

There is an example script located in the `+Scripts+` repository folder.   You may copy this script to your `/innerspace/Scripts` folder and then edit it in order to test isxSQLite functionality.   The script is heavily commented, so users are encouraged to read through it before asking questions.



Documentation
-------------
The source code is designed to be the primary documentation for ISXSQLite; however, users are encouraged to read through 'ISXSQLiteChanges.txt` and `+Scripts+\sqlite.iss`.  These files will be updated if/when new features are added to the extension.



Copyright
---------
ISXSQLite is Copyright 2011-2012 [Amadeus@isxGames.com](http://www.isxgames.com/)  Permission to use the source code in this file is granted under the Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit http://creativecommons.org/licenses/by/3.0/ for a summary of what rights are granted under this license.