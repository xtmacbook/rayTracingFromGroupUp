#include "World.h"
#include "src/build/BuildDragon.h"


std::string g_resource_path = "D:/workSpace/RayTraceGroundUp-master";

int main(){
    World w;        // construct a default world object
    w.build();
    w.camera_ptr->render_scene(w);
    return 0;
}
