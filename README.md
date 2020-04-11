# Unreal Collaboration
Demonetisation of networking components for Unreal project.

### What's This?
This project demonstrates the features of **Unreal Collaboration** on top of Unreal's first person example project. 

For deployment, we clients will download their executable from [Unreal Selector](https://gitlab.donald108.com/university/unreal-selector) & a dedicated servers will be available for clients to connect to and explore together (see **Build**).
The server is therefore a dedicated server which will run in a Linux environment, and the clients will be built for several platforms. 

To build the project, if you are planning on cross-compiling to Linux [install Unreal from source](https://docs.unrealengine.com/en-US/GettingStarted/DownloadingUnrealEngine/index.html), and generate the project files.

### Build
To build an **Unreal Collaboration** server, please do the following:
1. First setup the project for packaging
    * Setup the Gamemode to use your Blueprinted extensions of the following: `NetworkCharacter` as the default pawn, `NetworkPlayerController` as the default Player Controller, and `NetworkHUD` as the default HUD
    * Setup the *Entry* level to use `NetworkStartScriptActor` as it's parent class
    * *Entry* and *Transation* levels must be added. All maps must be selected to be cooked
    * Everything should be setup to allow for cooking (for example, one should not be referening `UBlueprint` within code as these are not explicitly packaged (only the generared code is) and instead should use Blueprints to select the Blueprint to use)
2. Once the project is setup for packaging, follow the instructions for deployment using [Unreal Selector](https://gitlab.donald108.com/university/unreal-selector) on Windows or Linux below

#### Client
1. Compile & run the project using the `Development Editor` build
2. Package the project for the target client architecture(s), save in a known location (e.g. `Build`) using File -> Package Project in the editor
3. Move the *contents* of `Build/${OS_NAME}NoEditor/` to its own folder (***the name of this folder should be the name of the final project***), which can be used for server distrubution.

#### Server
1. Compile & run the project using the `Development Editor` build
2. Package the project for the target server architecture, save in a known location (e.g. `Build`) using File -> Package Project in the editor
3. Back in the project solution, select `Development Server`
4. Right click on the solution, and click Build
5. Setup execute permissions of the server in `Binaries/${OS_NAME}/${PROJECT_NAME}Server`
6. Move the built server executable from `Binaries/${OS_NAME}/${PROJECT_NAME}Server` to `Build/${OS_NAME}NoEditor/${UNREAL_PROJECT_NAME}/Binaries/${OS_NAME}/`
7. Move the *contents* of `Build/${OS_NAME}NoEditor/` to its own folder (***the name of this folder should be the name of the final project***), which can be used for client distrubution (by zipping it).