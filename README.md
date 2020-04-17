# Unreal Collaboration
Demonetisation of networking components for Unreal project.

### What's This?
This project demonstrates the features of **Unreal Collaboration** on top of Unreal's first person example project. It features sign management, multiplayer & VR support, and administration through [Unreal Selector](https://gitlab.donald108.com/university/unreal-selector).

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

### Connections we use
For our functionality with [Unreal Selector](https://gitlab.donald108.com/university/unreal-selector), we use some external connections:
* Client connecting to server: The client finds the server IP address through Unreal Selector, and uses that IP address to connect.
* Client connecting to Unreal Selector writes a file (`Content/info.json`) containing its URL, the title of the game, the uses unreal selector session & the port the server is on.

### Notes for Testing
#### Multiplayer
When testing multiplayer, you should ensure that `Dedicated Server` is on - unless you want to *see* what is going on in the server, where you should turn off `Dedicated Server`, and set the number of players to >1.
You may consider turning off `Use Single Process` in advanced play settings, ensure when doing this you select `Play as Client`. This means that the server will be `fork()`ed from the main process and can run in isolation.

#### Virtual Reality
When testing in VR, ensure that `Use Single Process` is off initially. After launching, you can turn `Use Single Process` back on - however there will be no VR display. To get it displayed, start play again.
Starting with `Use Single Process` on crashes one of the rendering threads (`SharedPointer.h:851` will fail its assertion) and gives you no information of what happened (apart from `DepthSwapChain` was `NULL`).
You can also try another round-about method, but this doesn't always work: launch the game with `Use Single Process` on, allow the main process to crash, and *without changing code* relaunch and close the new server that is created.