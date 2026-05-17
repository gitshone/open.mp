# open.mp client — architecture

Windows-only, x86, C++20 client that replaces `samp.dll` as the in-process
modification for `gta_sa.exe`. The official open.mp client; the only target
needed to play on an open.mp server — no SA-MP installation, no SA-MP
launcher.

## Goals

1. **Drop-in protocol compatibility with SA-MP servers.** Anything an
   open.mp server (or a stock SA-MP server) sends on the wire is rendered
   identically.
2. **Standalone distribution.** Single installer ships `openmp.exe`
   (loader), `openmp-client.dll` (the mod), and the resources it needs.
3. **Modern stack.** No DXUT, no D3DXSprite, no D3DXFont, no hand-rolled
   detour machinery. C++20, MinHook, Dear ImGui, spdlog, toml++.

Scope is locked to what the existing open.mp server emits today. New
components and protocol features come after this client is shipped and
tested.

## Non-goals

- **No legacy network compatibility shims beyond what SA-MP servers
  already speak.** If a feature requires a non-open.mp protocol dialect,
  it does not belong here.
- **No reuse of the `SA-MP/` decompilation.** That repository is a
  derivative of proprietary closed-source SA-MP binaries; its license
  lineage is incompatible with open.mp's MPL 2.0. All client code is
  written clean-room against public DirectX 9 / Win32 documentation,
  open.mp's own `Shared/NetCode/` headers, and public GTA SA modding
  references.
- **No non-Windows hosts.** The mod target is `gta_sa.exe`, Win32 only.

## Stack

| Concern              | Choice                                           |
| -------------------- | ------------------------------------------------ |
| Language             | C++20 (Server stays C++17 for ABI; Client is leaf) |
| Function detours     | MinHook 1.3.4                                    |
| Overlay rendering    | Dear ImGui 1.91.5 (D3D9 + Win32 backends)        |
| Logging              | spdlog 1.15.0                                    |
| Config               | toml++ 3.4.0                                     |
| Networking           | RakNet (open.mp's MPL'd fork via Shared/NetCode) |
| Threading            | `std::jthread`, `std::stop_token`                |
| Build deps           | CMake FetchContent (no submodules)               |

D3DXSprite, D3DXFont, DXUT and ID3DXMatrix were deprecated by Microsoft
in 2010; nothing in the client uses them. Hand-written `VirtualProtect`
byte-patching is replaced by MinHook trampolines so detours can be
re-targeted at runtime and uninstalled cleanly.

## Layout

```
Client/
├── CMakeLists.txt            ; gate + FetchContent + add_subdirectory
├── ARCHITECTURE.md
├── Source/                   ; → openmp-client.dll
├── Loader/                   ; → openmp.exe
├── Installer/                ; NSIS script
└── Resources/                ; loading screen image, fonts, etc.
```

### `Source/`

| Subdir       | Responsibility                                              |
| ------------ | ------------------------------------------------------------ |
| `Hooks/`     | Direct3DCreate9 / WndProc / game-function detours, all through MinHook. |
| `Render/`    | `Overlay` — owns the Dear ImGui context and the DX9/Win32 backends. |
| `UI/`        | `ChatWindow`, `Scoreboard`, `Dialog`, `LoadingScreen`, `TextDraw`. Each describes a widget; Overlay drives the ImGui frame. |
| `Network/`   | `NetClient` (RakNet peer on a `std::jthread`) + `RpcDispatcher` (256-slot id → handler table over `std::span<const std::uint8_t>`). |
| `Game/`      | `GameSA` — the only module allowed to touch game memory. |
| `client_core`| Composition root. Owns each subsystem and the global lifecycle. |
| `dll_main`   | Spawns a worker thread on `DLL_PROCESS_ATTACH` because the loader's `LoadLibraryW` runs under loader lock. |

### `Loader/`

`openmp.exe` parses `--host / --port / --nick / --pass / --gta-dir`
(plus an `--omp-uri omp://…` form registered as a URL protocol handler),
detects the GTA install (CLI → env → registry), exports the join
parameters as `OMP_*` env vars, then `CreateProcessW(CREATE_SUSPENDED)`
→ `VirtualAllocEx` → `WriteProcessMemory` → `CreateRemoteThread(LoadLibraryW)`
→ `ResumeThread`. No `samp.dll`, no DLL search-order tricks.

### `Installer/`

NSIS, driven from CMake's `ClientInstaller` target. Drops the loader +
DLL + resources, registers the `omp://` URL protocol, writes Uninstall
registry keys and a Start Menu entry. Requires `makensis` on `PATH`.

## Protocol parity

The server hasn't changed. `Shared/NetCode/*.hpp` already defines the
SA-MP wire protocol; the new client links against those headers
directly. RPC IDs the client must implement at minimum:

| Direction | ID  | RPC                       | Source                          |
| --------- | --- | ------------------------- | ------------------------------- |
| C→S       | 25  | `PlayerConnect`           | `Shared/NetCode/core.hpp:33`    |
| S→C       | 137 | `PlayerJoin`              | `Shared/NetCode/core.hpp:87`    |
| S→C       | 61  | `ShowDialog`              | `Shared/NetCode/dialog.hpp:19`  |
| C→S       | 62  | `OnPlayerDialogResponse`  | `Shared/NetCode/dialog.hpp:44`  |
| S→C       | 134 | `PlayerShowTextDraw`      | `Shared/NetCode/textdraw.hpp:19`|
| S→C       | 135 | `PlayerHideTextDraw`      | `Shared/NetCode/textdraw.hpp:72`|

`Network/packet_handlers.cpp` is the single registration point.

### Identifying the client to the server

`PlayerConnect` carries `IsUsingOfficialClient` and `OmpVersion`
(`Shared/NetCode/core.hpp:41-42`). The client sets both so open.mp
servers can tell official-client traffic from stock SA-MP traffic if
they choose to. Stock SA-MP servers ignore the field.

## Threading

- **Game thread** (gta_sa main thread). All hook callbacks run here:
  D3D9 `Present`/`EndScene`/`Reset`, `WndProc`, game function detours.
  ImGui drawing happens here.
- **Network thread**. `NetClient::worker_` (a `std::jthread`) owns the
  RakNet peer, polls every ~5 ms, hands decoded packets to the
  dispatcher's queue.
- **Bootstrap thread**. Spawned from `DllMain` to escape loader lock.
  Initialises `ClientCore`, then exits — it does not persist.

## Lifecycle

```
openmp.exe
   │ CreateProcess(SUSPENDED) gta_sa.exe
   │ inject openmp-client.dll
   │ ResumeThread
   ▼
gta_sa.exe
   ├── DllMain(ATTACH) ──▶ spawn bootstrap thread ──▶ ClientCore::initialize
   │                                                    ├── GameSA::detect
   │                                                    ├── MH_Initialize
   │                                                    ├── GameHooks::install
   │                                                    ├── WndProcHook::install
   │                                                    └── D3D9DeviceHook::install
   ├── first D3D9 frame ──▶ Overlay::onDeviceAcquired
   │                          (ImGui_ImplDX9_Init + ImGui_ImplWin32_Init)
   ├── LoadingScreen visible (custom open.mp branding)
   ├── NetClient::connect(ConnectArgs from Config)
   ├── PlayerConnect(RPC 25) → server
   ├── PlayerJoin(RPC 137)  ← server → LoadingScreen::Done
   ├── steady state: UI driven by S→C RPCs, input/keyboard by WndProc
   │
   └── DllMain(DETACH) ──▶ ClientCore::shutdown ──▶ MH_Uninitialize
```

## Build

```powershell
mkdir build
cd build
cmake .. -A Win32 -T ClangCL -DBUILD_CLIENT=ON
cmake --build . --config RelWithDebInfo --target Client Loader
```

Installer (requires `makensis` on `PATH`):

```powershell
cmake .. -A Win32 -T ClangCL -DBUILD_CLIENT=ON -DBUILD_CLIENT_INSTALLER=ON
cmake --build . --target ClientInstaller
```

Requirements:

- 32-bit toolchain. `gta_sa.exe` is x86.
- Internet access on first configure for `FetchContent` to pull MinHook,
  Dear ImGui, spdlog and toml++.

## Style

All Client source files follow the open.mp Server conventions:

- MPL 2.0 header block on every file.
- Tab indentation, Allman braces, left-pointer alignment, no namespace
  wrapping of subsystem classes — `.clang-format` at the repo root is
  authoritative.
- `///` Doxygen comments on public class members; `//` only for
  implementation notes that document a non-obvious *why*.
- `snake_case` filenames (`client_core.hpp`), `PascalCase` class names
  (`ClientCore`), `camelCase` member functions and variables.

## Clean-room policy

Pull requests must not include code, structures, or addresses copied
from any decompilation of SA-MP or any closed-source SA-MP binary. The
`SA-MP/` sibling repo is reverse-engineering work and is not an
acceptable upstream. Where behaviour parity is needed, derive it from
observed network/file output of an open.mp server plus public Win32 and
DirectX documentation.

## Milestones

Implementation order, each milestone end-to-end testable against a
local open.mp server:

| # | Milestone           | Status                              | Done when                                                   |
| - | ------------------- | ----------------------------------- | ----------------------------------------------------------- |
| 1 | First frame         | Code written, **awaiting Windows verification** | Loader injects DLL; D3D9 captured; ImGui renders a watermark on the menu. |
| 2 | Join a server       | Not started                         | RakNet peer connects, `PlayerConnect` → `PlayerJoin`, chat lines arrive. |
| 3 | In-world parity     | Not started                         | Player + vehicle sync, dialogs, scoreboard, textdraws.      |
| 4 | Feature parity      | Not started                         | Objects, pickups, checkpoints, gangzones, 3D text labels, audio streams. |

## Current status

Last working session left the tree at the point below. Pick up from
"Next, on Windows".

### Implemented (real code)

| Module                                          | What it does                                                                                   |
| ----------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| `Loader/main.cpp`, `inject.cpp`, `game_detect.cpp` | Full process loader. Detects GTA via CLI → env → registry, spawns suspended, remote-thread `LoadLibraryW`, resumes. |
| `Source/dll_main.cpp`, `client_core.{hpp,cpp}`  | Bootstrap thread out of `DllMain`; constructs subsystems; wires D3D9 callbacks; MinHook init/shutdown. |
| `Source/config.{hpp,cpp}`                       | Loads `openmp-client.toml` (next to the DLL) with env-var overlay.                             |
| `Source/Hooks/d3d9_device_hook.{hpp,cpp}`       | Dummy-device vtable read; MinHook detours on Present (17) / EndScene (42) / Reset (16); first-EndScene captures the live device and focus HWND; routes per-frame and lost/reset events to the overlay. |
| `Source/Hooks/wnd_proc_hook.{hpp,cpp}`          | Deferred subclass: `attach(HWND)` is called from inside the D3D9 onDeviceAcquired callback; trampoline forwards to `ImGui_ImplWin32_WndProcHandler` first, then to any overlay handler, then to the original WndProc. |
| `Source/Render/overlay.{hpp,cpp}`               | Owns the Dear ImGui context plus its DX9/Win32 backends; render() draws the M1 watermark plus calls each UI subsystem's draw().                                                                                  |

### Stubs (compile, do nothing meaningful yet)

- `Source/Hooks/game_hooks.{hpp,cpp}` — no GTA function detours installed yet.
- `Source/UI/*` — `ChatWindow`, `Scoreboard`, `Dialog`, `LoadingScreen`, `TextDraw` all have correct state-management methods but their `draw()` bodies are TODO.
- `Source/Network/net_client.{hpp,cpp}` — `connect()` starts the jthread, the run loop just sleeps; no RakNet peer yet.
- `Source/Network/packet_handlers.cpp` — empty registration; target wiring is commented inside.
- `Source/Game/game_sa.{hpp,cpp}` — `detect()` returns true if `gta_sa.exe` is loaded; no checksum/version gate yet.

### Next, on Windows

1. **Configure and build M1.**
   ```powershell
   cd open.mp
   mkdir build
   cd build
   cmake .. -A Win32 -T ClangCL -DBUILD_CLIENT=ON
   cmake --build . --config RelWithDebInfo --target Loader Client
   ```
   First configure pulls MinHook 1.3.4, Dear ImGui 1.91.5, spdlog 1.15.0 and toml++ 3.4.0 via FetchContent. The Client subtree forces `CXX_STANDARD 20` on the targets so the Server's C++17 default stays untouched.

2. **Place `openmp-client.dll` and `openmp.exe` together** (CMake puts them in `build/Client/Source/RelWithDebInfo/` and `build/Client/Loader/RelWithDebInfo/` respectively; copy one next to the other, or use an output-dir setting).

3. **Launch.**
   ```powershell
   .\openmp.exe --nick test --gta-dir "C:\Path\to\GTA San Andreas"
   ```
   `--host`/`--port`/`--pass` are accepted but unused by M1 (no network yet).

4. **Expected outcome.** The GTA main menu loads with a small semi-transparent box in the top-left reading `open.mp client 0.1.0`.

5. **If it doesn't:** check `openmp-client.log` (written by spdlog next to the DLL). Likely failure modes, in order of probability:
   - `MH_CreateHook failed for one or more D3D9 entries` — MinHook couldn't patch the function prologue. Some d3d9.dll shipping with newer Windows is hot-patched by overlays; rebuild with `MH_CreateHookApi` instead of vtable-derived addresses.
   - `dummy CreateDevice failed` — try `D3DCREATE_HARDWARE_VERTEXPROCESSING` instead of `SOFTWARE` in `d3d9_device_hook.cpp::install`. Some drivers refuse software VP for `NULLREF`.
   - `SetWindowLongPtrW(GWLP_WNDPROC) failed` — likely a Windows version where the game's window is created on a different thread; in that case fall back to `SetWindowsHookEx(WH_CALLWNDPROC, ...)`.
   - `Direct3DCreate9 returned null` — `d3d9.dll` couldn't be resolved at all; should never happen on a Windows host with DirectX runtime.

## Open questions (revisit after milestone 4)

- D3D9 device-capture strategy: `Direct3DCreate9` detour vs. dummy
  proxy DLL placed beside `gta_sa.exe`. MinHook handles either.
- CJK / IME input: ImGui routes `WM_IME_*`, but `ChatWindow` and
  `Dialog` need composition-string handling on top.
- Audio backend choice (SA-MP shipped BASS; modern alternatives like
  miniaudio exist).
