# isxSQLite — Development Guide (for Claude)

> **What this file is.** A self-contained working reference for developing **isxSQLite** — the InnerSpace extension that exposes **SQLite 3** databases to **LavishScript** via datatypes and a TLO. It is written for fast recall while assisting on the **C++ source**, not as end-user scripting docs. Read this first; it replaces re-reading the whole project.
>
> **Authoritative source (precedence high → low):**
> 1. The **live source tree** `C:\Dev\InnerSpace\isxSQLite` — ground truth. The user edits files live; **always re-Read/Grep before asserting current contents or editing.** This guide is a point-in-time synthesis.
> 2. The companion guides for the sibling projects: `C:\Dev\InnerSpace\isxPantheon\ISXPantheon_Guide.md` (the single-file master-guide model this follows) and `C:\Dev\InnerSpace\ISXIM\ISXIM_Guide.md` (another small extension on the same framework). The `CppSQLite3*` wrapper (bundled in `SQLite\`) is documented in the libisxgames guide.
> 3. The global / project `CLAUDE.md` and the memory files in `C:\Users\Andy\.claude\projects\C--Dev---Claude-AI--\memory\`.
>
> **The single most important accuracy habit:** every datatype member/method you cite MUST exist in the source — verify the name against BOTH the member/method enum (each `DataTypes\*Type.h`) AND the `GetMember`/`GetMethod` switch (`*Type.cpp`) AND the `TypeMember`/`TypeMethod` registration in the ctor. Do not author `${X.Member}` examples by analogy. (Good news for this project: unlike ISXIM, **every** enum entry here is fully registered and handled — see §5 — but verify before you trust.)

---

## 0. The 60-second orientation

isxSQLite is an **ISXDK35 InnerSpace extension DLL** built on the **isxGames** framework (the same scaffolding as ISXEQ2 / isxPantheon / ISXIM). It attaches to **no game** — it is a thin LavishScript-facing wrapper around the bundled **CppSQLite3** C++ wrapper over the **SQLite 3** amalgamation. It:

- Lets a script **open named SQLite databases** (file-backed or in-memory) via `${SQLite.OpenDB[name,file]}`, which hands back a **`sqlitedb`** object.
- From a `sqlitedb` you **run DML** (`:ExecDML`, `:ExecDMLTransaction`), **run a SELECT** (`.ExecQuery[...]` → a **`sqlitequery`**), or **snapshot a whole table** (`.GetTable[...]` → a **`sqlitetable`**).
- A `sqlitequery` is a **forward cursor** (`:NextRow` / `.LastRow` / `:Reset`); a `sqlitetable` is a **random-access in-memory result set** (`:SetRow[n]`). Both expose per-field accessors (`.GetFieldValue`, `.GetFieldName`, `.FieldIsNULL`, …).
- All live objects are owned by three **global containers** (`gDatabases`, `gQueries`, `gTables`) keyed by name (DB) or integer ID (query/table); the datatypes are thin handles that look up into those maps every call.

**No game memory, no offsets, no detours, no mirror classes** — `RegisterDetours()`/`RegisterTriggers()` are empty stubs. The "Patcher" auto-update machinery is only compiled in when `USE_LIBISXGAMES` is defined (Amadeus's official-release build); the public **`Release` build bypasses it entirely**.

**The work loop:** add/extend a SQLite feature → add a **member** (read/return) or **method** (action) in the relevant `DataTypes\*Type.cpp` → add the enum entry in the matching `*Type.h` **AND** the `TypeMember`/`TypeMethod` call in that header's ctor (registration is a *separate step* from the `case`) → test with a `.iss` script (the canonical one is `+Scripts+\sqlite.iss`).

---

## 1. Identity, paths, build

### Paths
| Thing | Path |
|---|---|
| Project root | `C:\Dev\InnerSpace\isxSQLite` |
| Solution / project | `…\isxSQLite.sln` / `…\isxSQLite.vcxproj` |
| Datatypes (the heart) | `…\DataTypes\` (5 `*Type.cpp`/`.h` pairs) |
| Vendored SQLite + CppSQLite3 wrapper | `…\SQLite\` (third-party — do not document here; CppSQLite3 is covered in the libisxgames guide) |
| Vendored ISXDK | `…\ISXDK\35\` (framework SDK headers/libs) |
| Vendored isxGames lib | `…\libisxgames\` (only used when `USE_LIBISXGAMES` is defined) |
| Sample script | `…\+Scripts+\sqlite.iss` (heavily commented tutorial — quote the literal `+Scripts+` folder name) |
| Install staging | `…\Install\` (NSIS installer, manifests, popup XML) |

### Identity (`Defines.h`)
- `EXTENSION_NAME "isxSQLite"`, `EXTENSION_CLASS isxGamesExtension` (only `#define`'d under `USE_LIBISXGAMES`).
- Version is built from `_EXT_VER_YEAR/MONTH/DAY` = **2020.08.12** (`__isxSQLiteVersion` = the digits concatenated, `_EXT_VER_STR` = dotted). This is a **mature, low-churn** codebase (copyright 2011–2016; last version bump 2020). The script-facing `${ISXSQLite.Version}` returns the **product version from the DLL's `VersionInfo` resource**, not `_EXT_VER_STR`.
- `TESTCALLS` (default **0**) gates per-call logging macros `TESTCALLS_LOG_GETMEMBER/GETMETHOD/TOTEXT` (compile to nothing when 0). With `TESTCALLS=1`, `TESTCALLS_TOFILE` (default 1) sends the spew to a file via `DebugSpew` instead of the debugger. Every `GetMember`/`GetMethod`/`ToText`/TLO body opens with one of these macros.

### The `USE_LIBISXGAMES` split (read this before judging any `#ifdef`)
The single most important build fact: **two worlds, selected by `USE_LIBISXGAMES`.**
- **Defined** (Amadeus's official-release build): pulls the real `<isxGames.h>` library, links curl/ssl/zlib/tidy/ws2_32, enables `.NET 2.0/3.5` checks, the `dotnet isxSQLitePatcher` auto-updater (`RunThePatcher`, `Pulse.cpp`), and the `DebugSpew`/`GetURL` console commands. `Initialize2()` sets `gRunPatcherNow = true` and post-init waits for the patcher.
- **NOT defined** (the public open-source `Release`/`Release|x64` configs — the default): includes `libisxgames\libisxgames.h` instead, **skips the patcher entirely** — `Initialize2()` calls `isxSQLitePostInitialize(0,NULL,NULL)` straight away. Most `#ifdef USE_LIBISXGAMES` blocks (patcher, .NET gate, HTTP libs) are inert here. **When debugging a load issue in the public build, ignore all patcher/`RunThePatcher` code.**

### Build configs (`.vcxproj`) — **PlatformToolset v140** (VS2015), `ConfigurationType = DynamicLibrary`, `CharacterSet = MultiByte`
Four configurations, all named "Release" (there is **no Debug** config committed):
- **`Release|Win32`** and **`Release|x64`** — the public builds (no libisxgames). **Use these.** Per the README, set the project Output Directory to `/innerspace/Extensions/ISXDK35` (Win32) or `/innerspace/x64/Extensions/ISXDK35` (x64).
- **`Release - with libisxgames|Win32`** / **`…|x64`** — Amadeus's official-release builds (`USE_LIBISXGAMES`).
- README says "designed for no-hassle compiling using Visual Studio 2015; all required libraries and headers are included in the repo." `isxSQLite.cpp` `#pragma comment(lib,...)`s `delayimp` always, `isxdk` in the public build (or `isxGames`/`isxdk_md`/curl/ssl/zlib/tidy/ws2_32 under libisxgames).

---

## 2. Directory & file map

Legend: ✅ = active isxSQLite code · ⚙️ = framework/lifecycle scaffolding (edit rarely) · 💤 = stub / inert / vendored.

```
isxSQLite\
  Defines.h                  ⚙️ version digits, TESTCALLS flag + log macros
  isxSQLite.h                ⚙️ primary header — every .cpp includes it; pulls libisxgames + all project headers
  isxSQLite.cpp              ⚙️ THE lifecycle: Initialize / Initialize2 / PostInitialize / Shutdown / Register*/UnRegister* (X-macro driven)
  Pulse.h / Pulse.cpp        ⚙️ Pulse() (per-frame, mostly unload-gating); RunThePatcher() (libisxgames-only)
  Services.h / Services.cpp  ⚙️ ISXDK service connect/register (Pulse/Memory/HTTP/Triggers/Modules/System + the "SQLite Service" stub)
  Globals.h / Globals.cpp    ✅ event-id globals, the gDatabases/gQueries/gTables containers + counters, gQuietMode, datatype ptr defs
  Internals.h                ✅ RDT_* return-type consts (STRING/INT/DOUBLE/INT64) + FreeClear<M>() map-deleter template
  Utilities.h / Utilities.cpp ✅ the SQLite plumbing: OpenDatabase/OpenTable/ExecQuery/ExecDML/CloseDatabase + Close/FinalizeAll* + format()/ProcessMainXMLSettings()
  Commands.h / Commands.cpp  💤 X-macro COMMAND list; only "sqlite"->CMD_TestSQLite (an EMPTY test stub). DebugSpew/GetURL are libisxgames-only.
  TopLevelObjects.h / .cpp   ✅ TLO list ("SQLite"->TLO_SQLite) + the TLO_SQLite body (sentinel)
  ExtTopLevelObjects.h / .cpp ✅ ext-TLO list ("ISXSQLite"->TLO_isxSQLite) + body (sentinel) — registered immediately, persists whole load
  DataTypes.h                ✅ includes DataTypeList.h + all 5 DataTypes\*Type.h headers
  DataTypeList.h             ✅ X-macro DATATYPE registration list (isxSQLiteType is "ext"; the other 4 are normal)
  DataTypes\
    isxSQLiteType.cpp/.h     ✅ isxsqlite datatype — extension status/quietmode (the ${ISXSQLite} surface)
    SQLiteType.cpp/.h        ✅ sqlite datatype — the factory/entry point (${SQLite}: OpenDB, GetQueryByID, Escape_String)
    SQLiteDBType.cpp/.h      ✅ sqlitedb datatype — an open database (ExecQuery/GetTable/ExecDML/ExecDMLTransaction/Close)
    SQLiteQueryType.cpp/.h   ✅ sqlitequery datatype — a forward cursor over a SELECT result
    SQLiteTableType.cpp/.h   ✅ sqlitetable datatype — a random-access in-memory table snapshot
  SQLite\                    💤 VENDORED: sqlite3 amalgamation + CppSQLite3 wrapper (CppSQLite3.h/.cpp) — third-party, not documented here
  ISXDK\35\                  💤 VENDORED: ISXDK framework SDK (headers + libs)
  libisxgames\               💤 VENDORED: isxGames library extraction (only compiled when USE_LIBISXGAMES)
  +Scripts+\sqlite.iss       ✅ canonical sample/tutorial script (also mirrored under Install\Scripts\)
  Install\                   ⚙️ NSIS installer, manifests, isxSQLitepopup.xml
```

> **Where the real wrapping happens:** the 5 `DataTypes\*Type.cpp` files are pure LavishScript glue. The actual SQLite calls (`open`, `execDML`, `execQuery`, `getTable`, `tableExists`, field accessors) go through the vendored **`CppSQLite3DB` / `CppSQLite3Query` / `CppSQLite3Table`** classes (in `SQLite\CppSQLite3.h`). The thin layer in between — `Utilities.cpp`'s `OpenDatabase/OpenTable/ExecQuery/ExecDML/CloseDatabase` — does the `try/catch`, fires error/status events, and inserts the new object into the right global container.

---

## 3. The extension lifecycle (exact wiring)

Everything is registered through **X-macro lists** re-`#include`d with different macro definitions at the declare / register / unregister sites — identical pattern to ISXEQ2 / ISXIM.

### 3.1 `isxGamesExtension::Initialize(ISInterface*)` (`isxSQLite.cpp`)
1. Stash `pISInterface`; set `gExtensionLoading = true`; bail if `pISInterface` is null.
2. (libisxgames only) compute `ModulePath` = InnerSpace path + `\Extensions\`.
3. Refuse to load on **InnerSpace build < 5065** (prints a message, opens the console, returns false).
4. (libisxgames only) `.NET 2.0/3.5` availability gate — sets `gDoUnloadExtension` and bails if absent.
5. `RegisterExtension()` — writes isxSQLite's filename/path/version into the InnerSpace extension set so it can be loaded by name and update-checked.
6. **Resolve the LS primitive types** — `pStringType`, `pMutableStringType`, `pIntType`, `pInt64Type`, `pBoolType`, `pFloat64Type`, `pFloat64PtrType`, `pIndexType`, `pMutableStringType`, etc. via `pISInterface->FindLSType(...)`. These are the `Dest.Type` values every `GetMember` assigns. **(Declared in libisxgames, not in any isxSQLite header — grepping the project for their declaration finds nothing; that's expected.)**
7. `ConnectPulseService()` then `ConnectServices()` (Memory/HTTP/Triggers/Modules/System).
8. **`RegisterExtDataTypes()` + `RegisterExtTopLevelObjects()`** — registers the **ext**-scoped surface (`isxsqlite` datatype + `${ISXSQLite}` TLO) that stays alive the whole time the extension is loaded.
9. `RegisterPersistentClass("isxSQLite")` (for .NET scripts).
10. Register events: `isxGames_onHTTPResponse`, `isxSQLite_onErrorMsg`, `isxSQLite_onStatusMsg`, `isxSQLite_onUpdateComplete` (attached to `isxSQLitePostInitialize`), `isxSQLite_onDoShutdown` (attached to `isxSQLiteonDoShutdown`).
11. Open `isxSQLite.xml` (`OpenSettings`), ensure a `General` settings set exists, `ProcessMainXMLSettings()` (reads only `"Use Test Version"` → `gUseTestVersion`).
12. `Initialize2()`.

### 3.2 `Initialize2()` and `isxSQLitePostInitialize()`
- **`Initialize2()`** — in the **public build** (no libisxgames) calls **`isxSQLitePostInitialize(0,NULL,NULL)` directly**. In the libisxgames build it instead sets `ExtensionName`/`ExtensionMajorVersion` and `gRunPatcherNow = true` (post-init then fires later, after the patcher, via the `isxSQLite_onUpdateComplete` event / `RunThePatcher`).
- **`isxSQLitePostInitialize()`** — guarded by `if (gExtensionLoadDone) return;`. Registers the **stateful** surface: `RegisterCommands()` → `RegisterAliases()` → `RegisterDataTypes()` → `RegisterTopLevelObjects()` → `RegisterServices()` → `RegisterTriggers()` → `RegisterDetours()`. Sets `gExtensionLoadDone = true`, clears `gExtensionLoading`, prints the `isxSQLite v<version>` banner. `RegisterDataTypes()` also `SetPersistentClass(pisxSQLiteType, isxSQLiteClass)` for .NET.

> **Two-phase registration.** `RegisterExtDataTypes`/`RegisterExtTopLevelObjects` run in `Initialize` (so `${ISXSQLite}` works even mid-patch); `RegisterDataTypes`/`RegisterTopLevelObjects`/`RegisterCommands` run later in `PostInitialize` (so `${SQLite}` and the `sqlite*` datatypes come online only after load completes). The `EXT_DTYPES_ONLY` / `IGNORE_EXT_DTYPES` macro guards in `DataTypeList.h` split `isxSQLiteType` (ext) from the other four.

### 3.3 `Shutdown()`
If `gExtensionLoadDone`: disconnect+unregister services, then (unless a pre-unload already ran) unregister TLOs/datatypes/aliases/commands/detours/triggers; then always unregister the ext TLOs/datatypes. Then **unload the XML set, and tear down all live SQLite state in order: `FinalizeAllQueries()` → `FinalizeAllTables()` → `CloseAllDatabases()` → `FreeClear(gDatabases)` / `FreeClear(gQueries)` / `FreeClear(gTables)`** (the `FreeClear<M>` template `delete`s each map value then clears). Detach the two event targets, invalidate the persistent class, print `isxSQLite UNLOADED` (unless already unloading).

> **`FreeClear` only `delete`s — it does not call SQLite `close()`/`finalize()`.** That's why Shutdown calls `CloseAllDatabases`/`FinalizeAll*` first (which call the CppSQLite3 `close()`/`finalize()`), *then* `FreeClear` to delete the C++ objects and clear the maps. Don't reorder these.

### 3.4 The Pulse chain (`Pulse.cpp` / `Services.cpp`)
- `ConnectPulseService()` connects to the `"Pulse"` service; `PulseService(...)` fires `Pulse()` on `PULSE_PULSE` (every frame).
- **`Pulse()` is almost entirely unload-gating.** It decrements `gWaitPulses`; handles `gDoUnloadExtension` (schedules `ext -unload isxSQLite` via a timed command) and the pre-unload path (unregister-but-stay-loaded); (libisxgames only) runs the patcher when `gRunPatcherNow`; then returns early if `!gExtensionLoadDone`. **There is no per-frame SQLite work** — the region below the gate is empty. SQLite calls happen synchronously inside `GetMember`/`GetMethod` on the LS query thread, not on Pulse.

### 3.5 Services (`Services.cpp`)
Connects to the standard ISXDK services (Memory/HTTP/Triggers/Modules/System) and registers one **custom** service from the X-macro list: **`"SQLite Service"` → `SQLiteService` (`hSQLiteService`)**. All the service callbacks (`MemoryService`, `TriggerService`, `HTTPService`, `ModulesService`, `SystemService`, `SQLiteService`) are **empty handlers / stubs** — boilerplate copied from the isxGames extension template. The `SQLite Service` does nothing beyond the `CLIENTADDED`/`CLIENTREMOVED` no-ops; it exists as a template example, not a working feature.

### 3.6 Framework-provided globals (NOT declared in isxSQLite headers)
Flags/helpers isxSQLite uses that come from `<isxGames.h>` / libisxgames / the ISXDK (grepping the project for their *declaration* finds nothing — expected): `gExtensionLoadDone`, `gExtensionLoading`, `gDoUnloadExtension`, `gUnloadingExtension`, `gDidPreUnload`, `gUseTestVersion`, `gRunPatcherNow`, `pExtension`, `pISInterface`, the `p*Type` LS-type pointers, `ModulePath`/`ModuleFileName`, `IsNumber`, `IsAbsolutePath`, `IsConsoleOpen`, `DebugSpew`, `VersionInfo`, `ISXGames::GetTempBuffer`, `utf8string`, and the SQLite/CppSQLite3 classes (from `SQLite\`).

---

## 4. How isxSQLite wraps SQLite (the object model)

The data flow is **factory → handle → result set**, with all live objects owned by global containers, not by the datatype variables.

### 4.1 The global containers (`Globals.cpp`)
| Container | Type | Key | Holds | Counter |
|---|---|---|---|---|
| `gDatabases` | `unordered_map<string, CppSQLite3DB*>` | DB **name** (the 1st arg to `OpenDB`) | open databases | — |
| `gQueries` | `map<int, CppSQLite3Query*>` | integer **ID** | live SELECT cursors | `gQueriesCounter` (monotonic) |
| `gTables` | `map<int, CppSQLite3Table*>` | integer **ID** | in-memory table snapshots | `gTablesCounter` (monotonic) |

**The datatype variables are thin handles into these maps:**
- `sqlitedb`'s backing `ObjectData.CharPtr` is a `strdup`'d **DB name**; every call does `gDatabases.find(name)`.
- `sqlitequery` / `sqlitetable`'s backing `ObjectData.Int` is the **integer ID**; every call does `gQueries.find(id)` / `gTables.find(id)`.
- `${SQLite}` and `${ISXSQLite}` are **sentinels** — their TLOs just set `Dest.DWord=1`, and their `GetMember`/`GetMethod` only check `ObjectData.Ptr` is non-zero (no real backing object).

This handle-into-a-global-map design means a `sqlitedb`/`sqlitequery`/`sqlitetable` LS variable **stays valid across script scopes as long as the underlying object lives in its map** — you can reconstruct one from its `ID` (e.g. `variable sqlitedb DBCopy = "PlayerInfoDB"`). It also means **leaks are the script's responsibility**: a query/table that is never `:Finalize`d stays in `gQueries`/`gTables` until `Shutdown`.

### 4.2 Opening a database (`OpenDatabase`, `Utilities.cpp`)
`${SQLite.OpenDB[name,file]}` → `OpenDatabase(name, file)`:
1. Resolve the path: if `file` is not absolute, prefix `ModulePath\` (the Extensions dir); else use it verbatim. **isxSQLite does NOT create directories** — a subdir in the path must already exist.
2. `":Memory:"` / `":memory:"` → an in-memory DB (treated as "file does not exist").
3. `pDatabase->open(fullpath)` inside try/catch → on `CppSQLite3Exception`, fire `isxSQLite_onErrorMsg`, `delete`, return null.
4. **If the file is new**, apply a fixed PRAGMA bundle: `encoding=UTF-8`, `auto_vacuum=1`, `cache_size=2048`, `page_size=4096`, `synchronous=NORMAL`→then `OFF`, `journal_mode=OFF`, `temp_store=MEMORY`. (These tune for speed over crash-safety — `journal_mode=OFF` + `synchronous=OFF` means a crash mid-write can corrupt the DB. Worth remembering if a user reports corruption.)
5. `gDatabases.insert({name, pDatabase})`; return the pointer. The caller (`SQLiteType::OpenDB`) first rejects a **duplicate name** with an error event.

### 4.3 Executing — query vs table vs DML
From a `sqlitedb`:
- **`.ExecQuery[sql]`** → `ExecQuery(pDB,sql)` (`Utilities.cpp`): `pDB->execQuery(sql)`. If it throws OR the result is **immediately `eof()` (no rows)**, fires `isxSQLite_onErrorMsg` (the empty-result case uses ErrCode `-1`, message "Query returned no results"), finalizes, returns `false`/0. Otherwise `gQueriesCounter++`, inserts into `gQueries`, returns the new ID. The member hands back a `sqlitequery` whose `Dest.Int` = that ID.
- **`.GetTable[name]`** → `OpenTable(pDB,name)`: if `name` is a real table, runs `select * from <name> order by 1;` and `getTable(...)`; otherwise treats `name` as a custom SELECT DML and `getTable(name)` directly. On success `gTablesCounter++`, inserts into `gTables`, returns the ID → a `sqlitetable`.
- **`:ExecDML[dml]`** → `ExecDML(pDB,dml)`: single `execDML`, try/catch → error event. Returns bool.
- **`:ExecDMLTransaction[index:string]`** → takes a LavishScript **`index:mutablestring`** variable name, `DataParse`s it, validates type/subtype, wraps the items in `BEGIN TRANSACTION;` … `END TRANSACTION;`, executing each `utf8string` item. Faster for bulk inserts; any failure fires an error event and aborts.

### 4.4 Reading a query vs a table
- **`sqlitequery` is a forward cursor.** `.NumRows` (`CppSQLite3Query::NumRows()`), `.NumFields`, field accessors apply to the **current row**; `:NextRow` advances, `.LastRow` = `eof()`, `:Reset` rewinds. Field access by **index or name**. `:Finalize` `delete`s the query and erases it from `gQueries` (fires a status event).
- **`sqlitetable` is a fully-materialized random-access result set.** `.NumRows` / `.NumFields` are the whole snapshot; **`:SetRow[n]`** moves the active row (0-based), then field accessors read that row. Field access is **by index or name**. `:Finalize` `delete`s + erases from `gTables`. (No `NextRow`/`LastRow`/`Reset` — use the `SetRow` + `NumRows` loop pattern, as `SpewTable` in the sample does.)

### 4.5 Field value typing (`GetFieldValue`) — and a real bug
`GetFieldValue[fieldindex-or-name {, typekeyword}]` is the workhorse. Intended: an optional 2nd arg `"int"`/`"float"`/`"double"`/`"int64"` selects the return type (default = string). The `CppSQLite3` accessors are `getIntField`/`getFloatField`/`getInt64Field`/`fieldValue` (string), each overloaded for index (`atoi`) or name. **See §9 for the verified fall-through bug** that makes the typed paths mostly return strings anyway.

---

## 5. The datatype & TLO surface (VERIFIED against source)

> Every member/method below was cross-checked against **all three**: the enum in `DataTypes\*Type.h`, the `TypeMember`/`TypeMethod` call in that header's ctor, and the `case` in the `*Type.cpp` `GetMember`/`GetMethod` switch. **Unlike ISXIM, isxSQLite has NO implemented-but-unregistered members** — every enum entry is registered and handled. (The one near-miss: `SQLiteType`'s `SQLiteTypeMethods` enum is **empty** and its `GetMethod` switch is commented out — `${SQLite}` has members only.)

### 5.1 `isxsqlite` datatype — `${ISXSQLite}` (ext TLO, always available)
Backing: `TLO_isxSQLite` sets `Dest.DWord=1; Dest.Type=pisxSQLiteType` (sentinel; `GetMember` only checks `ObjectData.Ptr != 0`). Impl in `DataTypes\isxSQLiteType.cpp`. `ToText` = `"isxsqlite"`. **Note the casing**: the registered ext-TLO name is `"ISXSQLite"`, the datatype's LSType name is `"isxsqlite"`.

| Member | Returns | Source |
|---|---|---|
| `Version` | string | `VersionInfo::GetProductVersion()` (DLL resource version) |
| `IsReady` | bool | `gExtensionLoadDone` |
| `IsLoading` | bool | `gExtensionLoading` |
| `InQuietMode` | bool | `gQuietMode` |

| Method | Args | Effect |
|---|---|---|
| `QuietMode` | *(none)* | **toggles** `gQuietMode` (no on/off arg — it flips) and prints ACTIVE/INACTIVE |

### 5.2 `sqlite` datatype — `${SQLite}` (TLO `TLO_SQLite`)
Backing: `TLO_SQLite` sets `Dest.DWord=1; Dest.Type=pSQLiteType` (sentinel). Impl in `DataTypes\SQLiteType.cpp`. `ToText` = `"sqlite"`. This is the **factory / entry point**. **Members only — no methods** (the methods enum is empty).

| Member | Args | Returns | Notes |
|---|---|---|---|
| `OpenDB` | `[name,file]` (exactly 2) | `sqlitedb` | opens/creates the DB, registers it under `name`; rejects a name already in `gDatabases` (error event). `Dest` = `strdup(name)`. |
| `GetQueryByID` | `[id]` (numeric) | `sqlitequery` | looks up an existing query ID in `gQueries`; null if absent |
| `Escape_String` | `[str]` | string | `sqlite3_mprintf("%q", str)` — SQL-escapes a string for safe literal embedding |

> `OpenDB`'s file arg: relative → under `…\Extensions\`; `":memory:"` → in-memory; directories are not auto-created (§4.2).

### 5.3 `sqlitedb` datatype — a handle to an open database
Backing: `ObjectData.CharPtr` = the DB **name**; every call does `gDatabases.find(name)` and bails if absent. Impl in `DataTypes\SQLiteDBType.cpp`. `ToText` = the DB name. Supports `FromText`/`Set` so a script can do `variable sqlitedb DB = "name"` to reconstruct a handle.

| Member | Args | Returns | Notes |
|---|---|---|---|
| `ID` | — | string | the DB name (use `.ID(exists)` to validate the handle — null if the DB isn't open) |
| `ExecQuery` | `[sql]` (1) | `sqlitequery` | runs a SELECT; null if it errors or returns **no rows** (§4.3) |
| `TableExists` | `[name]` (1) | bool | `CppSQLite3DB::tableExists` |
| `GetTable` | `[name-or-sql]` (1) | `sqlitetable` | materializes a table or custom-SELECT snapshot (§4.3) |

| Method | Args | Effect |
|---|---|---|
| `Close` | — | `CloseDatabase` — `close()`, `delete`, erase from `gDatabases`, status event |
| `ExecDML` | `[dml]` (exactly 1) | single `execDML`; error event on wrong argc or SQL error |
| `ExecDMLTransaction` | `[index:string varname]` (1) | wraps an `index:mutablestring` of DML in BEGIN/END TRANSACTION (§4.3) |
| `Set` | `[name]` | sets the handle's backing name (the `FromText` path; how `variable sqlitedb X = "name"` works) |

> `Set` is intercepted at the top of `GetMethod` (`if (pMethod->ID == Set && argc>0) return FromText(...)`) **before** the `gDatabases` validity check — so `:Set` works even on an as-yet-unbound handle.

### 5.4 `sqlitequery` datatype — a forward cursor over a SELECT
Backing: `ObjectData.Int` = the query **ID**; every call does `gQueries.find(id)`. Impl in `DataTypes\SQLiteQueryType.cpp`. `ToText` = the ID as a string. `Set`/`FromText` sets `ObjectData.Int = atoi(arg)`.

| Member | Args | Returns | Notes |
|---|---|---|---|
| `ID` | — | int | the query ID |
| `NumRows` | — | int | `CppSQLite3Query::NumRows()` |
| `NumFields` | — | int | `numFields()` |
| `GetFieldName` | `[colindex]` (numeric) | string | `fieldName(n)` |
| `GetFieldIndex` | `[colname]` | int | `fieldIndex(name)`; -1 on error |
| `GetFieldDeclType` | `[colindex]` (numeric) | string | `fieldDeclType(n)` — the column's declared SQL type |
| `GetFieldType` | `[colindex]` (numeric) | int | `fieldDataType(n)` — SQLite runtime type code (1=INT,2=FLOAT,3=TEXT,4=BLOB,5=NULL); -1 on error |
| `GetFieldValue` | `[index-or-name {,type}]` | string/int/int64/float | current row's field; type keyword in **`argv[1]`** (`int`/`float`/`double`/`int64`); default string. **See §9 fall-through bug.** |
| `FieldIsNULL` | `[index-or-name]` (1) | bool(int) | `fieldIsNull(...)`; -1 on error |
| `LastRow` | — | bool | `eof()` — true when the cursor is past the last row |

| Method | Args | Effect |
|---|---|---|
| `NextRow` | — | `nextRow()` — advance the cursor (error event on throw) |
| `Reset` | — | `Reset()` — rewind to the first row |
| `Finalize` | — | `finalize()`, `delete`, erase from `gQueries`, status event. **Always finalize when done — else it leaks until Shutdown.** |
| `Set` | `[id]` | sets `ObjectData.Int = atoi(id)` (reconstruct a handle from an ID) |

### 5.5 `sqlitetable` datatype — a random-access in-memory table snapshot
Backing: `ObjectData.Int` = the table **ID**; every call does `gTables.find(id)`. Impl in `DataTypes\SQLiteTableType.cpp`. `ToText` = the ID as a string. `Set`/`FromText` sets `ObjectData.Int = atoi(arg)`.

| Member | Args | Returns | Notes |
|---|---|---|---|
| `ID` | — | int | the table ID |
| `NumRows` | — | int | `CppSQLite3Table::numRows()` (the whole snapshot) |
| `NumFields` | — | int | `numFields()` |
| `GetFieldName` | `[colindex]` (numeric) | string | `fieldName(n)` |
| `GetFieldValue` | `[index-or-name {,type}]` | string/… | value at the **current row** (set via `:SetRow`). **See §9 — this one has a 2nd bug: it reads the type keyword from `argv[0]`, not `argv[1]`.** |
| `FieldIsNULL` | `[index-or-name]` (1) | bool(int) | `fieldIsNull(...)`; -1 on error |

| Method | Args | Effect |
|---|---|---|
| `SetRow` | `[n]` (numeric) | `setRow(n)` — set the active row (0-based) before reading fields |
| `Finalize` | — | `finalize()`, `delete`, erase from `gTables`, status event. **Always finalize when done.** |
| `Set` | `[id]` | sets `ObjectData.Int = atoi(id)` |

> `sqlitetable` has **no `NextRow`/`LastRow`/`Reset`** (those are query-only). Iterate with `for (rCount:Set[0]; ${rCount} < ${Table.NumRows}; rCount:Inc) { Table:SetRow[${rCount}]; … }`.

### 5.6 Worked example (grounded against `+Scripts+\sqlite.iss` — every token verified)
```
; --- open / create a DB, run DML, read it back ---
variable sqlitedb DB
DB:Set[${SQLite.OpenDB["PlayerInfoDB","PlayerInfoDB.sqlite3"]}]
if !${DB.ID(exists)}
  return                                              ; OpenDB failed

if !${DB.TableExists["Friends"]}
  DB:ExecDML["create table Friends (key INTEGER PRIMARY KEY, name TEXT, level INTEGER);"]

DB:ExecDML["insert into Friends (name,level) values ('Lax',13);"]

; --- query (forward cursor) ---
variable sqlitequery Q = ${DB.ExecQuery["SELECT * FROM Friends WHERE level=13;"]}
if ${Q.NumRows} > 0
{
  do
  {
    echo "${Q.GetFieldValue["name",string]} (lvl ${Q.GetFieldValue["level"]})"
    Q:NextRow
  }
  while !${Q.LastRow}
}
Q:Finalize                                            ; MUST finalize or it leaks

; --- table snapshot (random access) ---
variable sqlitetable T = ${DB.GetTable["Friends"]}
variable int r
for (r:Set[0] ; ${r} < ${T.NumRows} ; r:Inc)
{
  T:SetRow[${r}]
  echo "${T.GetFieldName[1]}: ${T.GetFieldValue[1]}"
}
T:Finalize

DB:Close
```
(Note: `${Q.GetFieldValue["age",double]}` will, due to the §9 fall-through bug, return the value as a *string* rather than a coerced double — the sample's `.Precision[2]` happens to still work on the string. Don't promise reliable typed coercion until that bug is fixed.)

---

## 6. Commands, events, settings

### 6.1 Commands (`Commands.h` / `Commands.cpp`)
`COMMAND(name, cmd, parse, hide)`, registered in `RegisterCommands()`:
- **`sqlite` → `CMD_TestSQLite`** — an **empty stub** (`return 0;`), described in-source as "an easy way to test code while the extension is loaded." Not a user feature. **There is no console command surface for actual SQLite use — everything is driven through the datatypes/TLOs.**
- `DebugSpew` / `GetURL` — registered only under `USE_LIBISXGAMES` (framework helpers, not part of the public build).

### 6.2 Events (registered in `Initialize`, fired throughout `Utilities.cpp` + the `*Type.cpp` files)
| Event | argc | Fired when | argv |
|---|---|---|---|
| `isxSQLite_onErrorMsg` | 2 | any SQLite error (open/DML/query/field/transaction failure, empty result set, duplicate DB name, bad argc) | `[ErrCode, ErrMsg]` — ErrCode is the SQLite error code or `"-1"` when there's no unique code |
| `isxSQLite_onStatusMsg` | 1 | non-error status (DB closed, query/table finalized) | `[StatusMsg]` |
| `isxGames_onHTTPResponse` | (framework) | only via the libisxgames `GetURL` command | — |
| `isxSQLite_onUpdateComplete` | — | libisxgames patcher completion → triggers `isxSQLitePostInitialize` | — |
| `isxSQLite_onDoShutdown` | — | request to unload (sets `gDoUnloadExtension`) | — |

`onFrameEnds` is declared (`Globals.h`) but **never registered or fired** — dead. `isxSQLiteSpawnApplication` is declared in `Globals.h` but **not defined anywhere** in the project (dead decl).

A script attaches with `Event[isxSQLite_onErrorMsg]:AttachAtom[...]`. The error event is the primary debugging channel; with `${ISXSQLite.InQuietMode}` off, the same messages also `printf` to the console.

### 6.3 Settings (`isxSQLite.xml` via `ProcessMainXMLSettings`)
Reads exactly one setting from the `General` set: **`"Use Test Version"`** → `gUseTestVersion` (only meaningful in the libisxgames patcher path; selects the test manifest URL). No SQLite credentials or DB paths are persisted — databases are opened entirely via `${SQLite.OpenDB[...]}` args at runtime.

---

## 7. Utilities & internals

- **`Utilities.cpp`** — the only file with real SQLite logic outside the datatypes: `OpenDatabase`, `OpenTable`, `ExecQuery`, `ExecDML`, `CloseDatabase` (each try/catch + error/status event + container insert/erase), the bulk teardown helpers `CloseAllDatabases`/`FinalizeAllQueries`/`FinalizeAllTables`, `ProcessMainXMLSettings`, and a hand-rolled `format()`/`format_arg_list()` (`_vscprintf`+`_vsnprintf_s` — deliberately avoiding a boost dependency so the project compiles "out of the box").
- **`Internals.h`** — `RDT_STRING/INT/DOUBLE/INT64` (1/2/3/4) return-type selectors for `GetFieldValue`, and the `FreeClear<M>()` template that `delete`s every value in a map then clears it (used in `Shutdown`).
- **`Utilities.h`** — note the local `namespace LavishScript2 { typedef void ILS2Array; typedef void LS2Exception; };` and the `LS2Exception **ppException=0` trailing params on the SQLite helpers. These exception-out params are **threaded through but never actually used** (always passed `0`/`nullptr`); they're a forward-looking hook for LavishScript2 integration that was never wired up. Don't assume they carry error info.

---

## 8. Conventions (when writing/editing isxSQLite C++)

This is a **2011–2020-era codebase** — match what's already there rather than imposing the newer isxPantheon style. Observed conventions:
- **Allman braces**, **tabs**, no cosmetic line-wrapping (wide lines are fine).
- Original code uses **`unsigned int`/`int`/`MAX_PATH`/`CHAR`** and `0`/`nullptr` (it does mostly use `nullptr`, not `NULL`). The user's global fixed-width-type preference (`uint32_t`, etc.) applies to **new/edited** code you author; do **not** mass-rewrite existing types unless asked, and never touch tokens inside `"..."` literals (the PRAGMA bundle, the SQL keyword strings, the `"int"`/`"float"`/`"double"`/`"int64"` type keywords).
- **X-macro lists are the registration mechanism** (`DataTypeList.h`, `TopLevelObjects.h`, `ExtTopLevelObjects.h`, `Commands.h`, `Services.h`). To add a datatype member/method: edit the enum **and** add the `TypeMember`/`TypeMethod` call in the ctor **and** add the `case` — all three. (isxSQLite is currently consistent about this; keep it that way.)
- **Datatype impl pattern:** `GetMember`/`GetMethod`/`ToText` first resolve+null-check the backing object (sentinel types check `ObjectData.Ptr`; DB looks up `gDatabases` by `CharPtr`; query/table look up by `Int`), then `switch` on `pMember->ID`/`pMethod->ID`, set `Dest.Type` to the right `p*Type`, return true; false for unknown/failed. The `Set` method is special-cased **before** the validity check so an unbound handle can still be `:Set`.
- **`Dest` fields:** `Dest.Int` + `pIntType`/`pBoolType`; `Dest.Int64` + `pInt64Type`; `Dest.Float64` + `pFloat64Type`/`pFloat64PtrType`; `Dest.ConstCharPtr = ISXGames::GetTempBuffer.Convert(std::string)` + `pStringType` for any returned string (this is the safe way to hand back a `std::string` — copies into a managed temp buffer); `Dest.CharPtr = strdup(...)` + `p<Other>Type` for an object handle whose backing is a string (the `sqlitedb` name).
- **Errors go through `isxSQLite_onErrorMsg`** (argc 2: code, message) plus a `gQuietMode`-gated `printf`. Status (close/finalize) goes through `isxSQLite_onStatusMsg` (argc 1). Match this pattern for any new failable operation — don't just `printf`.
- **Every new SQLite call must be in a `try { … } catch (CppSQLite3Exception& e) { … }`** that fires the error event with `e.errorCode()`/`e.errorMessage()`. That's the house pattern in every `*Type.cpp` accessor.
- **Git:** never `git commit` unless the user explicitly says to. Volunteer a concise one-line message (≤72 chars + `Co-Authored-By`) after a substantial edit.
- **isxSQLite is open-source (CC BY 3.0)** — but the sibling ISX projects are closed; don't cross-cite closed-source ISX internals into anything distributable.

---

## 9. Consolidated gotchas / hard-won notes

- ⚠️ **`GetFieldValue` type-coercion is broken by missing `break`s.** In `SQLiteTableType::GetFieldValue` **and** partly `SQLiteQueryType::GetFieldValue`, the `switch(ReturnDT)` cases lack `break` statements and **fall through to `default` (string)**. In the **Table** version *none* of `RDT_DOUBLE`/`RDT_INT64`/`RDT_INT` breaks → every typed request returns a **string**. In the **Query** version only `RDT_DOUBLE` and `RDT_INT64` have `break`; `RDT_INT` falls through to default → `[…,int]` returns a string. Net effect: typed numeric coercion is unreliable; scripts that "work" do so because LavishScript re-parses the string. If asked to fix, add `break;` to each case (and re-check the `Dest.Type` for the float path — see next note).
- ⚠️ **Table `GetFieldValue` reads the type keyword from the wrong argv.** Table uses `stricmp(argv[0],"int")` (the *field* arg) to detect the type keyword; Query correctly uses `argv[1]`. So even ignoring the fall-through, Table's optional type arg is wired to the wrong slot. The fix is `argv[1]` to match Query.
- ⚠️ **Float return uses a pointer type with a value.** Both `GetFieldValue` float paths set `Dest.Float64 = …` but `Dest.Type = pFloat64PtrType` (the *pointer* LS type, not `pFloat64Type`). Mostly moot today because of the fall-through, but a hazard if you fix the `break`s without also fixing the type.
- **No implemented-but-unregistered members** (the ISXIM failure mode does **not** occur here) — every enum entry has its `TypeMember`/`TypeMethod` + `case`. The only "missing" surface is intentional: `SQLiteType` has an **empty methods enum** and a commented-out `GetMethod` switch (`${SQLite}` is members-only).
- **A query with zero rows is treated as an error, not an empty result.** `ExecQuery` fires `isxSQLite_onErrorMsg` ("Query returned no results", code -1) and returns 0 when the SELECT is immediately `eof()`. So `${DB.ExecQuery[…]}` returning NULL can mean *either* a SQL error *or* simply "no matching rows" — a script can't distinguish them from the return value alone (check the error message).
- **Leaks are the script's job.** `sqlitequery`/`sqlitetable` objects persist in `gQueries`/`gTables` until `:Finalize` (or `Shutdown`'s `FinalizeAll*`+`FreeClear`). The sample script stresses this repeatedly. Same for `sqlitedb` (`:Close`).
- **Shutdown teardown order matters:** `FinalizeAllQueries` → `FinalizeAllTables` → `CloseAllDatabases` (these call the CppSQLite3 `finalize()`/`close()`) **then** `FreeClear` (which only `delete`s + clears the maps). `FreeClear` does *not* close/finalize. Don't reorder.
- **New DB files get speed-over-safety PRAGMAs** (`journal_mode=OFF`, `synchronous=OFF`). A crash mid-write can corrupt the file. This only applies to *newly created* files, not reopened ones.
- **`OpenDB` won't create directories.** A relative file path with a subdir (`"sqlite\\foo.sqlite3"`) requires the subdir to already exist; otherwise `open` fails. Relative paths resolve under `…\Extensions\` (or `…\x64\Extensions\` for x64).
- **`USE_LIBISXGAMES` is the master switch.** In the public `Release` build the patcher, .NET gate, HTTP service, and `DebugSpew`/`GetURL` commands are all `#ifdef`'d out; `PostInitialize` runs immediately from `Initialize2`. Don't chase patcher code when debugging the public build.
- **All service callbacks are empty stubs** (Memory/HTTP/Triggers/Modules/System/`SQLite Service`) — template boilerplate. `Pulse()` does no SQLite work either; everything happens synchronously inside `GetMember`/`GetMethod`.
- **Dead/unused symbols:** `onFrameEnds` (declared, never fired), `isxSQLiteSpawnApplication` (declared in `Globals.h`, never defined), `CMD_TestSQLite` (empty), the `LS2Exception**ppException` params (always 0). The `SetRow` error message in `SQLiteTableType` is mislabeled `"SQLiteQuery.SetRow::"` (copy-paste artifact).
- **Casing traps:** the ext-TLO registered name is **`ISXSQLite`** (all-caps SX), the datatype LSType name is `isxsqlite`; the factory TLO is `SQLite`, its datatype is `sqlite`. Members are case-sensitive as written (`Escape_String`, `GetQueryByID`, `FieldIsNULL`, `GetFieldDeclType`).
- **Subagents:** foreground by default; a background/resumed agent auto-denies un-pre-approved Edit/Write. `C:\Dev\InnerSpace\**` is allowlisted for Edit/Write. Quote the literal `+Scripts+` folder name in any shell path.

---
*Guide authored 2026-06-16 from a full read of the isxSQLite project (every framework/lifecycle file + all 5 `DataTypes\*Type.cpp`/`.h` pairs + `Utilities.cpp` + the `+Scripts+\sqlite.iss` sample; the vendored `SQLite\`/`ISXDK\`/`libisxgames\` trees were noted but not documented). Every datatype member/method was cross-checked against the enum, the ctor `TypeMember`/`TypeMethod` registration, AND the `GetMember`/`GetMethod` `case`. If this file ever contradicts the live source, the source wins — re-verify and fix this file.*
