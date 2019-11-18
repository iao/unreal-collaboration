# Networking Test
Simple test of networking within Unreal.

### What's this?
Starting from the fist person example which can be generated from Unreal, we have allowed multiplayer interaction through the use of **Replication**. 

Unreal uses an **authoritative server** model, this is a model in which the server controls the definitive state of the world, and clients send updates to their world to the server which will update all other clients of this. Unreal uses **Replication** to state that an object will be replicated to clients.

In this project, we demonstrate simple replication with the ball the player makes in `NetworkingTestCharacter.cpp`. We see that `onFire()` is called when the player hits the mouse button, however this doesn't directly spawn the projectile instead calls `ServerFire()` to spawn it. The function `ServerFire()` is only ran on the server because of the `UFUNCTION(Server, Reliable, WithValidation)` macro we find in `NetworkingTestCharacter.h`. This macro states that this function will only be ran on the server, **will** get called when a client says it should (uses TCP) & it is validated to ensure the client isn't cheating. We also see replication in `NetworkingTestProjectile.cpp`, here the projectile will only be destroyed by the server so that clients all have the same projectiles visible at once.

For deployment, we are expecting clients to download their executable from [Unreal Selector](https://gitlab.donald108.com/university/unreal-selector) & have dedicated servers available for clients to connect to and explore together.
The server is therefore a dedicated server which will run in a Linux environment, and the clients will be built for several platforms. 

To build the project, if you are planning on cross-compiling to Linux [install Unreal from source](https://docs.unrealengine.com/en-US/GettingStarted/DownloadingUnrealEngine/index.html), and generate the project files. Then build both a packaged version of the client from within the editor target, and build the server directly with the server target.