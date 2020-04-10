# Unreal Collaboration
Demonetisation of networking components for Unreal project.

### How does this work?
Starting from the fist person example which can be generated from Unreal, we have allowed multiplayer interaction through the use of **Replication**. 

Unreal uses an **authoritative server** model, this is a model in which the server controls the definitive state of the world, and clients send updates to their world to the server which will update all other clients of this. Unreal uses **Replication** to state that an object will be replicated to clients.

In this project, we demonstrate replication for many components. For example, the ball the player shoots in `NetworkingTestCharacter.cpp`. We see that `onFire()` is called when the player hits the mouse button, however this doesn't directly spawn the projectile instead calls `ServerFire()` to spawn it. The function `ServerFire()` is only ran on the server because of the `UFUNCTION(Server, Reliable, WithValidation)` macro we find in `NetworkingTestCharacter.h`. This macro states that this function will only be ran on the server, **will** get called when a client says it should (uses TCP) & it is validated to ensure the client isn't cheating. We also see replication in `NetworkingTestProjectile.cpp`, here the projectile will only be destroyed by the server so that clients all have the same projectiles visible at once.

For deployment, we are expecting clients to download their executable from [Unreal Selector](https://gitlab.donald108.com/university/unreal-selector) & have dedicated servers available for clients to connect to and explore together (see **build**).
The server is therefore a dedicated server which will run in a Linux environment, and the clients will be built for several platforms. 

To build the project, if you are planning on cross-compiling to Linux [install Unreal from source](https://docs.unrealengine.com/en-US/GettingStarted/DownloadingUnrealEngine/index.html), and generate the project files. Then build both a packaged version of the client from within the editor target, and build the server directly with the server target.

### Build
To build a server built off of this project, please do the following:
1. First setup the project for packaging
    * *Entry* and *Transation* levels must be added, and maps must be selected to be cooked
    * Everything should be setup to allow for cooking (for example, one should not be referening `UBlueprint` within code as these are not explicitly packaged (only the generared code is) and instead should use Blueprints to select the Blueprint to use)
2. Once the project is setup for packaging, follow the instructions for deployment on Windows or Linux

#### Linux
1. [Get the source for unreal engine](https://www.unrealengine.com/en-US/ue4-on-github)
2. Set the project to use the source build of Unreal
3. Make Blueprints from the classes available in Unreal Colloboartion.
4. Compile & run the project
5. Package the project for **BOTH** Linux and Windows, save in a known location
6. Back in the project solution, select `Development Server` as the target and `Linux` as the architecture and build
7. Copy the build project (`TODO:PATH`) to where you saved the packaged build (`/Packaged/Binaries/Linux/`)
8. Copy the packaged project to the deployment server
9. Setup execute permissions of the server
10. Run the server, and connect from your client

#### Windows
**Currently not supported**