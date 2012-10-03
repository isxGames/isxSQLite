/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#pragma once

#define RDT_STRING		1
#define RDT_INT			2
#define RDT_DOUBLE		3
#define RDT_INT64		4


template <typename M> void FreeClear( M & amap ) 
{
    for ( typename M::iterator it = amap.begin(); it != amap.end(); ++it ) 
	{
		if (it->second)
	        delete it->second;
    }
    amap.clear();
}

