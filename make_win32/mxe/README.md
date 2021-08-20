# Cross-compilation

Scripts in this directory allow to cross-compile part of CF from Linux to Windows, using a Docker container.

## Requirements

Docker installed.

## Quick run

From this directory, run `./build-all.sh`.

This will build the CF server (without plugins), and CRE.

Server is  `../../server/crossfire-server.exe`, CRE is `../../utils/cre/release/cre.exe`.

## Implementation choices

Everything is linked statically, to avoid having to find required `DLLs` to run the executable.

This of course makes executables bigger, but simplifies many things.

## Scripts

### docker-setup.sh

This scripts creates the Docker image that will be used to cross-compile.

It installs required packages and MXE in static mode.

The user will be `user`, with the same UID as yourself, to not mess with file permissions.

### docker-run.sh

This script starts the Docker container in which the compilation will occur.

The parent directory of the `server` code will be mounted as `/home/user/project`.
The assumption is that you have `arch` and `maps` at the same level as the server.

If arguments are provided, then they are considered as commands and run, then the container will exit.

### configure-cross-compile-win.sh

Merely calls `configure.sh` from the `server` root with the correct `host` key for compilation.

### build-cre.sh

Builds the `CRE` tool

### build-all-docker.sh

Script that should be run in Docker, to build everything.

It merely calls the previous `configure` and `build-cre` scripts.

### build-all.sh

Script to call from outside Docker, that will run `build-all-docker.sh` in a Docker container.
