;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; sqlite.iss  
;;;
;;; Tutorial script for isxSQLite
;;; by Amadeus
;;;
;;; This script is intended to show the majority of features made available by isxSQLite.   Commentary is found 
;;; throughout the script and should answer most all questions regarding this extension.
;;;
;;; NOTES:
;;; 1. This script does not 'delete' the sqlite3 database that it creates.  Therefore, every execution of this script
;;;    will add additional (identical) records to the same database.  (If you wish to get the same results each time, 
;;;    you will need to delete /innerspace/Extensions/PlayerInfoDB.sqlite3 after each script execution.)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;;; Global Variables
variable sqlitedb PlayerInfoDB

;;;;;;;;;;;;;;;;;;
;; EVENTS
atom(script) isxSQLite_onErrorMsg(int ErrorNum, string ErrorMsg)
{
	;;;;
	;;(NOTE:  Not all errors have a unique ErrorNum.  In these cases, it will be -1.   However, ALL errors will have a unique "ErrorMsg".)
	;;;;
	
	if (${ErrorNum} > 0)
		echo "[sqlite] <ERROR> (${ErrorNum}) ${ErrorMsg}"
	else
		echo "[sqlite] <ERROR> ${ErrorMsg}"
}

atom(script) isxSQLite_onStatusMsg(string StatusMsg)
{
	;;;;
	;; Any 'spew' from isxSQLite which is not an error, will be echoed through this event (i.e., results from SQLiteDB:Close)
	;;;;
	  
	;; 'Status' updates that go through this event are typically only needed for debugging purposes.  So feel free to comment out this echo to reduce
	;; unecessary spam
	echo "[sqlite] <STATUS> ${StatusMsg}"
}

atom(script) isxGames_onHTTPResponse(int Size, string URL, string IPAddress, int ResponseCode, float TransferTime, string ResponseText, string ParsedBody)
{
	;; ~ This event only fires for responses that occur due to the "GetURL" command being utilized
	;; ~ "Size" is in bytes
	;; ~ "URL" should match the URL issued with the "GetURL" command (unless modified by the server)
	;; ~ "ResponseText" is the entire response text unparsed (ie, including all html/xml tags)
	;; ~ "ParsedBody" will return the plain text of the <body> section of an html document.  However, at this time it only works for simple documents
	;; with only plain text between the <body></body> tags.  I will be improving upon this in the future.
}

;;;;;;;;;;;;;;;;;;
;; main()
function main(... Args)
{
	; If isxSQLite isn't loaded, then no reason to run this script.
	if (!${ISXSQLite(exists)})
		return

	; Make sure the extension is ready and is fully loaded
	if (!${ISXSQLite.IsReady})
	{
		do
		{
			waitframe
		}
		while !${ISXSQlite.IsReady}
	}
	  
	
	;Initialize/Attach the event Atoms that we defined previously
	Event[isxSQLite_onErrorMsg]:AttachAtom[isxSQLite_onErrorMsg]
	Event[isxSQLite_onStatusMsg]:AttachAtom[isxSQLite_onStatusMsg]	
	Event[isxGames_onHTTPResponse]:AttachAtom[isxGames_onHTTPResponse]
	
	
	; Now, since we're scripting ALL the events ...let's turn on QuietMode.  If you are
	; debugging new scripts, or are having problems, be sure to turn it off so that 
	; the extension will spew things to the console. 
	if (!${ISXSQLite.InQuietMode})
		ISXSQLite:QuietMode        
	


	;Tell the user that the script has initialized and is running!
	echo "[sqlite] -----> isxSQLite Sample Script (Using isxSQLite version ${ISXSQLite.Version}) <-----"
	
	;;;;;;;;
	;;; For the purposes of this sample, we are going to create an SQLite3 database called "PlayerInfoDB" (Filename: PlayerInfoDB.sqlite3)
	;;; This script uses the sqlitedb variable 'PlayerInfoDB' as a global variable (see top of the script); and then sets that variable here.
	;;; However, you could have just as easily declared the variable and set it at the same time here using something like this:
	;;;                 declare PlayerInfoDB sqlitedb ${SQLite.OpenDB["PlayerInfoDB","PlayerInfoDB.sqlite3"]}
	;;;
	;;; It is also important to realize that SQLite3 databases CAN reside entirely in memory.  Of course, if you do this, the data will only
	;;; be available during the current session (obviously, it's not saved to file.)   While this will not be useful to most, some may find 
	;;; instances where they need to store and retrieve data purely in the current session and do not need anything stored in a file.  To
	;;; open the DB entirely in memory, you would use ":Memory:" in place of "PlayerInfoDB.sqlite3"
	;;;
	;;;;;;;;
	PlayerInfoDB:Set[${SQLite.OpenDB["PlayerInfoDB","PlayerInfoDB.sqlite3"]}]
		
	if (${PlayerInfoDB.ID(exists)})
		echo "[sqlite] Database '${PlayerInfoDB.ID}' opened."
	else
	{
		echo "[sqlite] <ERROR> Failure to open database...ending script."
		return
	}
	;;;;;;;;
	;; Now we have either created or are accessing a file in your /innerspace/Extensions directory called 'PlayerInfoDB.sqlite3'.   We could
	;; have placed the file in a subdirectory using something like "sqlite\\PlayerInfoDB.sqlite3" for the second argument.  However, you must
	;; make sure that the directory already exists as isxsqlite will not create the directory for you.
	;;
	;; This script is making intentional use of the 'ID' members of each major datatype.   The reason for this is to illustrate how it can be
	;; used to verify that things are valid as well as to show how you can use this ID to create copies of the object in the future.  In other
	;; words, using this particular sqlitedb object as an example, now that you've opened it using the unique ID of 'PlayerInofDB', it will be
	;; available for your use via that ID until you either: 1) close the DB or 2) unload the extension.
	;;
	;; So, if I wanted to create another copy of of this sqlite3 database in a new variable, I could do something like this:
	;;                  variable sqlitedb DBCopy = "PlayerInfoDB"
	;; 
	;; Assuming that this database was properly "opened" previously, and has not been closed, this statement will work just fine.  However, for sanity,
	;; you should always double-check that the 'ID' member (exists).   'ID' will always return NULL if the database object is NULL.
	;;;;;;;;
	
	
	;;;;;;;;
	;; SQLite3 databases are designed to hold within them data containers called 'tables'.  An SQLite3 database can have as many tables as you want; however,
	;; each must have a unique name. 
	;;
	;; First, one must determine if the database has the proper schema in place.  If not, then we need to create the proper tables for use.  If the tables
	;; already exist, then there is no reason to do this again.   (See http://www.sqlite.org/lang_createtable.html)
	;;
	if (!${PlayerInfoDB.TableExists["Amadeus_Friends"]})
	{
		;; Since 'Amadeus_Friends' does not exist, we're going to assume that this is a new database and that none of the tables exist
		PlayerInfoDB:ExecDML["create table Amadeus_Friends (key INTEGER PRIMARY KEY, name TEXT, level INTEGER, age REAL, notes TEXT);"]
		PlayerInfoDB:ExecDML["create table Amadeus_Inventory (key INTEGER PRIMARY KEY, name TEXT, mass REAL, value REAL);"]
	
		;; Notice how the table names have a prefix of "MYNAME_".  It is useful to use prefixes so that table names stay 'unique' and that you can use a single
		;; database as much information as possible.
	}
	;; It is important to note that if you have a significant number of DML Statements to execute at once, you may wish to consider using
	;; the 'ExecDMLTransaction' method instead.  Three (as used above) is not enough to warrant its use; however, it is something to keep in
	;; mind.   (An example of using 'ExecDMLTransaction' will be shown later in this script.)
	;;;;;;;
	
	;;;;;;;
	;;;; [Add some records to the three tables in the sqlite3 database]   
	;; The sample script does this via one of two functions below.  You should only use ONE of the following function calls when testing with 
	;; this script.  However, both work, and both do exactly the same thing.   In actuality, "call AddRecordsViaTransaction" will be more efficient 
	;; because sqlite3 'transactions' are faster due to how they are processed. 
	;;
	;; How do you decide which to use in your scripts?   It's primarily a question of how many DML statements you're executing at once.  If you're doing
	;; a large number, you'll want to use a transaction.  If it's 1 to 5 (or so), then simply using the 'ExecDML' method should be fine.   A lot depends
	;; on how much data you're adding/retrieving, how many DML statements you're processing, disk speed, etc.   So, there is no simple answer to this.
	;; If you're not sure or want to make sure that you're always using the fastest means of reading from/writing to the database, then use 'ExecDMLTransaction'.
    
	call AddRecordsViaTransaction ${PlayerInfoDB.ID}
	;call AddRecords ${PlayerInfoDB.ID}


	;;;;;;;
	;;;; [Retrieve records]
	;; Important Definitions:
	;;  1.  Row   -- A 'line' of information in a table is called a "row".
	;;  2.  Query -- When using isxSQLite, the 'query' is what is returned to you after you request information from the database.  A query can consist of 
	;;               any number of rows.
	;; 
	;;  Therefore, when ask the database to return information, your script should be written so that it can handle any rows that might be returned.  
	;;
	;;  For more information on the DML for retrieving information from an sqlite3 database, see http://www.sqlite.org/lang_select.html.   There are a lot of 
	;;  VERY powerful options when using the 'SELECT' sqlite3 command.  So, the sky is the limit!
    
	declare Level13Friends sqlitequery ${PlayerInfoDB.ExecQuery["SELECT * FROM Amadeus_Friends where level=13;"]}
	
	if (${Level13Friends.NumRows} > 0)
	{
		echo "[sqlite] Amadeus has ${Level13Friends.NumRows} friends who are level 13."
		;; There are several ways to iterate here.  The do/while loop below is very efficient.  However, you could also do a 'for' (counter) loop based on 
		;; incrementing an integer variable until it reaches the "NumRows"
		do
		{
			echo "[sqlite] - ${Level13Friends.GetFieldValue["name",string]}"
			echo "[sqlite] -- Age:   ${Level13Friends.GetFieldValue["age",double].Precision[2]}"  
			echo "[sqlite] -- Notes: '${Level13Friends.GetFieldValue["notes",string]}'"
			Level13Friends:NextRow
		}
		while (!${Level13Friends.LastRow})
		;; If you ever wanted to reset the query back to the first row returned, you can use the 'Reset' method (Example:  Level13Friends:Reset)
	}
    
	Level13Friends:Finalize
	;;
	;; It is very important to "Finalize" a query once you're done using it.   Feel free to keep it around as long as you need it; however, to leave it "hanging"
	;; once you're done with it will create a memory leak.  Even if you decide to re-use an sqlitequery variable, you should "Finalize" it before setting it to 
	;; the results of a new sqlitedb.ExecQuery[].
	;;;;;;;
	

	;;;;;;;
  ;; [Exporting an entire table]
  ;; There may be a situation in which you would want to export an entire table.  isxSQLite provides a mechanism for doing this should you ever wish to export
  ;; your database as a specialy formatted file (i.e., comma-delimited), do some debugging, or manually mine for data throughout the entire table.
  ;;
  echo "[sqlite] Testing export of 'Amadeus_Inventory' table:"
  call SpewTable ${PlayerInfoDB.ID} "Amadeus_Inventory"
	
	
	
	;;;;;;;;
	;; Now that we are completely done with our sqlite3 database, it is time to 'Close' it.  isxSQLite3 will close all databases when it's unloaded; however,
	;; it is best if scripts take care of closing the databases they open.
	;;;;;;;;
	PlayerInfoDB:Close
		
	
	;We're done with the script, so let's detach all of the event atoms
	Event[isxSQLite_onErrorMsg]:DetachAtom[isxSQLite_onErrorMsg]
	Event[isxSQLite_onStatusMsg]:DetachAtom[isxSQLite_onStatusMsg]	
	Event[isxGames_onHTTPResponse]:DetachAtom[isxGames_onHTTPResponse]
	
	
	;Send a final message telling the user that the script has ended
	echo "[sqlite] -----> isxSQLite Sample Script END <-----"
}

;;;;;;;;;;;;;;;;;;
;; Utility Functions
function AddRecordsViaTransaction(string DatabaseID)
{
	variable sqlitedb DB = ${DatabaseID}
	if (!${DB.ID(exists)})
	{
		echo "[sqlite] <ERROR>  'AddRecordsViaTransaction' called with an invalid Database ID (${DatabaseID})"
		return
	}
	
	;;;;;
	;; See http://www.sqlite.org/lang_insert.html
	;;;;;
	variable index:string DML
	DML:Insert["insert into Amadeus_Friends (name,level,age,notes) values ('Cybertech', 100, 123543.25, 'CyberTech works on isxeve');"]
	DML:Insert["insert into Amadeus_Friends (name,level,age,notes) values ('Lax', 13, 123.65, 'Lax owns Lavishsoft');"]
	DML:Insert["insert into Amadeus_Friends (name,level,age,notes) values ('Doctor Who', 13, 995.1, 'The Doctor');"]

	DML:Insert["insert into Amadeus_Inventory (name,mass,value) values ('iPad', 1.35, 0.053);"]
	DML:Insert["insert into Amadeus_Inventory (name,mass,value) values ('Titan(EVE)',2278125000.0,70000000000.0);"]
	DML:Insert["insert into Amadeus_Inventory (name,mass,value) values ('Public Opinion', 234234653245.254, 0.0);"]
	
	DB:ExecDMLTransaction[DML]
	
	;; The "ExecDMLTransaction" METHOD of the 'sqlitedb' datatype inserts the the DML statement
	;; "BEGIN TRANSACTION;" prior to the ones you submit, and then includes "END TRANSACTION;" 
	;; after all of your statements have been submitted.   In other words, you do *NOT* need to 
	;; include these directives when using the "ExecDMLTransaction" method.	
}

function AddRecords(string DatabaseID)
{
	variable sqlitedb DB = ${DatabaseID}
	if (!${DB.ID(exists)})
	{
		echo "[sqlite] <ERROR>  'AddRecords' called with an invalid Database ID (${DatabaseID})"
		return
	}
	
	;;;;;
	;; See http://www.sqlite.org/lang_insert.html
	;;;;;
	DB:ExecDML["insert into Amadeus_Friends (name,level,age,notes) values ('Cybertech', 100, 123543.25, 'CyberTech works on isxeve');"]
	DB:ExecDML["insert into Amadeus_Friends (name,level,age,notes) values ('Lax', 13, 123.65, 'Lax owns Lavishsoft');"]
	DB:ExecDML["insert into Amadeus_Friends (name,level,age,notes) values ('Doctor Who', 13, 995.1, 'The Doctor');"]
	
	DB:ExecDML["insert into Amadeus_Inventory (name,mass,value) values ('iPad', 1.35, 0.0);"]
	DB:ExecDML["insert into Amadeus_Inventory (name,mass,value) values ('Titan (EVE)',2278125000.0,70000000000.0);"]
	DB:ExecDML["insert into Amadeus_Inventory (name,mass,value) values ('Public Opinion', 234234653245.254, 0.0);"]
}

function SpewTable(string DatabaseID, string TableName)
{
	variable int fCount = 0
	variable int rCount = 0
	variable sqlitedb DB = ${DatabaseID}
	if (!${DB.ID(exists)})
	{
		echo "[sqlite] <ERROR>  'AddRecords' called with an invalid Database ID (${DatabaseID})"
		return
	}
	if (!${DB.TableExists[${TableName}]})
	{
		echo "[sqlite] <ERROR>  'SpewTable' called with an invalid table name (${TableName})"
		return
	}
	
	declare Table sqlitetable ${DB.GetTable[${TableName}]}
	if (!${Table.ID(exists)})
	{
		echo "[sqlite] <ERROR>  'SpewTable' encountered an odd error where a table exists...but 'GetTable' was unable to retrieve it!"
		return
	}
	
	if (${Table.NumRows} > 0)
	{
		for (rCount:Set[0] ; ${rCount} < ${Table.NumRows} ; rCount:Inc)
		{
				Table:SetRow[${rCount}]
			
				for (fCount:Set[0] ; ${fCount} < ${Table.NumFields} ; fCount:Inc)
				{
						if (${Table.FieldIsNULL[${fCount}]})
							continue
							
						;; Since all we're doing is spewing contents, we can simply retrieve and print everything as a string type.  In other words, for 
						;; this particular routine, there is no 'type' argument necessary as we do not care if whether the return value is a string, int,
						;; float, etc..
						
						echo "[sqlite] -- ${rCount}. ${Table.GetFieldName[${fCount}]}: ${Table.GetFieldValue[${fCount}]}"
				}
		}
	}
		
	
	Table:Finalize
	;;
	;; It is very important to "Finalize" the table once you're done using it.   Feel free to keep it around as long as you need it; however, to leave it "hanging"
	;; once you're done with it will create a memory leak.  Even if you decide to re-use an sqlitetable variable, you should "Finalize" it before setting it to 
	;; the results of a new sqlitedb.GetTable[].
	;;;;;;;
	
}