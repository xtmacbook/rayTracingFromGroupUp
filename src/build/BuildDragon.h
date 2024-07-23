#include "World.h"
#include "Ambient.h"
#include "Pinhole.h"
#include "PointLight.h"
#include "Directional.h"
#include "RayCast.h"
#include "Whitted.h"
#include "Plane.h"
#include "MultiJittered.h"
#include "AmbientOccluder.h"
#include "Emissive.h"
#include "AreaLight.h"
#include "Rectangle.h"
#include "AreaLighting.h"
#include "Instance.h"
#include "Disk.h"
#include "Grid.h"
#include "CubicNoise.h"
#include "Image.h"
#include "FBmTextureRamp.h"
#include "InstanceTexture.h"
#include "Matte.h"
#include "SV_Matte.h"
#include "SV_Phong.h"
#include "Wood.h"
#include "Image.h"
#include "ImageTexture.h"
#include "SV_Reflective.h"
#include "SV_GlossyReflector.h"
#include "FBmTextureWrapped.h"
#include "Rosette.h"
#include "BeveledWedge.h"
#include "BeveledBox.h"
#include "Phong.h"
#include "SpherePartConcave.h"

#include <iostream>
#include <fstream>
using namespace std;

extern std::string g_resource_path;


#define TYPE 0
// 0: rosette
// 1: wedge
// 2: beveled box
// 3: scene
// 4: scene with grid

#if TYPE == 0
void
World::build(void) {
    int num_samples = 1;
    int a = 1;

    vp.set_hres(600 / a);
    vp.set_vres(400 / a);
    vp.set_samples(num_samples);

    tracer_ptr = new Whitted(this);

    Pinhole* pinhole_ptr = new Pinhole;
    pinhole_ptr->set_eye(50, 50, 30);
    pinhole_ptr->set_lookat(0);
    pinhole_ptr->set_view_distance(5000 / a);
    pinhole_ptr->compute_uvw();
    set_camera(pinhole_ptr);

    Directional* light_ptr2 = new Directional;
    light_ptr2->set_direction(20, 30, 30);
    light_ptr2->scale_radiance(2.0);
    //	light_ptr2->set_shadows(true);
    add_light(light_ptr2);


    // rosette parameters

    int		num_rings = 3;		// maximum of 6, default value is 3
    float	hole_radius = 0.75;
    float	ring_width = 1.0;
    float	rb = 0.1;			// bevel radius
    float	y0 = -0.25;			// minimum y value
    float 	y1 = 0.25;			// minimum y value


    Rosette* rosette_ptr = new Rosette(num_rings, hole_radius, ring_width, rb, y0, y1);

    // put a different random marble texture on each wedge

    // ramp image:

    Image* image_ptr = new Image;
    auto img = g_resource_path + std::string("/TextureFiles/ppm/BlueMarbleRamp.ppm");
    image_ptr->read_ppm_file(img.c_str());

    // marble texture parameters

    int num_octaves = 4;
    float lacunarity = 2.0;
    float gain = 0.5;
    float perturbation = 3.0;

    int num_objects = rosette_ptr->get_num_objects();

    for (int j = 0; j < num_objects; j++) {

        // noise:

        CubicNoise* noise_ptr = new CubicNoise;
        noise_ptr->set_num_octaves(num_octaves);
        noise_ptr->set_gain(gain);			// not relevant when num_octaves = 1
        noise_ptr->set_lacunarity(lacunarity);     // not relevant when num_octaves = 1


        // marble texture:

        FBmTextureRamp* marble_ptr = new FBmTextureRamp(image_ptr);
        marble_ptr->set_noise(noise_ptr);
        marble_ptr->set_perturbation(perturbation);


        // transformed marble texture

        InstanceTexture* wedge_marble_ptr = new InstanceTexture(marble_ptr);
        set_rand_seed(j * 10);
        wedge_marble_ptr->scale(0.25);
        wedge_marble_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));

        // marble material

        SV_Matte* sv_matte_ptr = new SV_Matte;
        sv_matte_ptr->set_ka(0.35);
        sv_matte_ptr->set_kd(0.75);
        sv_matte_ptr->set_cd(wedge_marble_ptr);

        rosette_ptr->store_material(sv_matte_ptr, j);	// store material in the specified wedge
    }

    rosette_ptr->setup_cells();
    add_object(rosette_ptr);
}
#endif // TYPE

#if TYPE == 1

void
World::build(void) {
    int num_samples = 1;
    int a = 4;

    vp.set_hres(500 / a);
    vp.set_vres(400 / a);
    vp.set_samples(num_samples);

    tracer_ptr = new RayCast(this);

    Pinhole* pinhole_ptr = new Pinhole;
    pinhole_ptr->set_eye(0, 40, 80); // (50, 40, 30)
    pinhole_ptr->set_lookat(0, 0, 0);
    pinhole_ptr->set_view_distance(4000 / a); // 4000
    pinhole_ptr->compute_uvw();
    set_camera(pinhole_ptr);

    Directional* light_ptr2 = new Directional;
    light_ptr2->set_direction(20, 30, 30);
    light_ptr2->scale_radiance(2.5);
    //	light_ptr2->set_shadows(true);
    add_light(light_ptr2);

    Matte* matte_ptr1 = new Matte;
    matte_ptr1->set_ka(0.25);
    matte_ptr1->set_kd(0.65);
    matte_ptr1->set_cd(0.5, 1, 0.5);	  // green

    // wedge1 parameters

    float y0 = -1.0;		// minimum y value
    float y1 = 2;			// maximum y value
    float r0 = 1.5;			// inner radius, 1.5
    float r1 = 3;			// outer radius
    float rb = 0.25;		// bevel radius, 0.25
    float phi0 = 0;		// minimum azimuth angle in degrees, 140
    float phi1 = 90;		// maximum azimuth angle in degrees, 350

    BeveledWedge* wedge_ptr1 = new BeveledWedge(y0, y1, r0, r1, rb, phi0, phi1);
    wedge_ptr1->set_material(matte_ptr1);
    add_object(wedge_ptr1);


    Matte* matte_ptr2 = new Matte;
    matte_ptr2->set_ka(0.25);
    matte_ptr2->set_kd(0.85);
    matte_ptr2->set_cd(1.0, 0.5, 0.0);	// orange

    // wedge2 parameters

    y0 = -1.5;		// minimum y value
    y1 = 1.25;		// minimum y value
    r0 = 0.5;		// inner radius
    r1 = 4.0;		// outer radius
    rb = 0.075;		// bevel radius
    phi0 = 110;		// minimum azimuth angle in degrees
    phi1 = 130;		// maximum azimuth angle in degrees

    BeveledWedge* wedge_ptr2 = new BeveledWedge(y0, y1, r0, r1, rb, phi0, phi1);
    wedge_ptr2->set_material(matte_ptr2);
    //	add_object(wedge_ptr2);


    Matte* matte_ptr3 = new Matte;
    matte_ptr3->set_cd(1, 1, 0.0);	// yellow
    matte_ptr3->set_ka(0.25);
    matte_ptr3->set_kd(0.85);

    // wedge3 parameters

    y0 = -0.75;		// minimum y value
    y1 = 0.5;		// minimum y value
    r0 = 1.25;		// inner radius
    r1 = 3.75;		// outer radius
    rb = 0.1;		// bevel radius
    phi0 = 0;		// minimum azimuth angle in degrees
    phi1 = 90;		// maximum azimuth angle in degrees

    BeveledWedge* wedge_ptr3 = new BeveledWedge(y0, y1, r0, r1, rb, phi0, phi1);
    wedge_ptr3->set_material(matte_ptr3);
    //	add_object(wedge_ptr3);
}

#endif // TYPE

#if TYPE == 2

void
World::build(void) {
    int num_samples = 1;
    int a = 4;

    vp.set_hres(600 / a);
    vp.set_vres(280 / a);
    vp.set_samples(num_samples);

    tracer_ptr = new RayCast(this);

    Pinhole* pinhole_ptr = new Pinhole;
    pinhole_ptr->set_eye(10, 15, 50);
    pinhole_ptr->set_lookat(0, 0.75, 0);
    pinhole_ptr->set_view_distance(4000 / a);
    pinhole_ptr->compute_uvw();
    set_camera(pinhole_ptr);

    PointLight* light_ptr1 = new PointLight;
    light_ptr1->set_location(10, 15, 20);
    light_ptr1->scale_radiance(3.0);
    //	light_ptr1->set_shadows(true);
    add_light(light_ptr1);

    Phong* phong_ptr = new Phong;
    phong_ptr->set_ka(0.3);
    phong_ptr->set_kd(0.75);
    phong_ptr->set_cd(0.7, 0.5, 0);		// orange
    phong_ptr->set_ks(0.2);
    phong_ptr->set_exp(3);

    float bevel_radius = 0.05;  // for all objects

    // box
    // the untransformed box is centered on the origin

    Point3D p0(-0.75, -1.125, -0.75);
    Point3D p1(0.75, 1.125, 0.75);

    Instance* box_ptr = new Instance(new BeveledBox(p0, p1, bevel_radius));
    box_ptr->rotate_y(-10);
    box_ptr->translate(2.5, 0.38, -1);
    box_ptr->set_material(phong_ptr);
    add_object(box_ptr);

    // ground plane

    Matte* matte_ptr = new Matte;
    matte_ptr->set_ka(0.5);
    matte_ptr->set_kd(0.85);
    matte_ptr->set_cd(0.25);

    Plane* plane_ptr = new Plane(Point3D(0, -0.75, 0), Normal(0, 1, 0));
    plane_ptr->set_material(matte_ptr);
    add_object(plane_ptr);
}

#endif // TYPE

#if TYPE == 3
void
World::build(void) {
    int num_samples = 16;
    int a = 1;

    vp.set_hres(1000 / a);
    vp.set_vres(500 / a);
    vp.set_samples(num_samples);

    tracer_ptr = new Whitted(this);

    Pinhole* pinhole_ptr = new Pinhole;
    pinhole_ptr->set_eye(0.0, 2.6, 6.8);
    pinhole_ptr->set_lookat(0.0, 0.0, -8);
    pinhole_ptr->set_view_distance(600 / a);
    pinhole_ptr->compute_uvw();
    set_camera(pinhole_ptr);

    PointLight* point_light_ptr1 = new PointLight;
    point_light_ptr1->set_location(0, 3, -5);
    point_light_ptr1->scale_radiance(2.0);
    point_light_ptr1->set_cast_shadow(true);
    add_light(point_light_ptr1);

    PointLight* point_light_ptr2 = new PointLight;
    point_light_ptr2->set_location(0, 3, 5);
    point_light_ptr2->scale_radiance(2.0);
    point_light_ptr2->set_cast_shadow(true);
    add_light(point_light_ptr2);

    ////////////////////////////////////////sky, SpherePartConcave///////////////////////////////////////

    SpherePartConcave* spc_sky_ptr = new SpherePartConcave(Point3D(0, 0, 0), 1, 0, 360, 0, 90);

    // image:

    Image* image_sky_ptr = new Image;
    //	image_sky_ptr->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/MorningSky.ppm");

    auto img = g_resource_path + std::string("/TextureFiles/ppm/EveningSky.ppm");
    image_sky_ptr->read_ppm_file(img.c_str());

    // image based texture:

    ImageTexture* texture_image_sky_ptr = new ImageTexture;
    texture_image_sky_ptr->set_image(image_sky_ptr);

    InstanceTexture* it_image_sky_ptr = new InstanceTexture(texture_image_sky_ptr);
    it_image_sky_ptr->scale(1000000);

    // material:

    SV_Phong* sv_phong_sky_ptr = new SV_Phong;
    sv_phong_sky_ptr->set_ka(0.1);
    sv_phong_sky_ptr->set_kd(0.25);
    sv_phong_sky_ptr->set_cd(texture_image_sky_ptr);
    sv_phong_sky_ptr->set_ks(0.1);
    sv_phong_sky_ptr->set_exp(20.0);


    Instance* instance_spc_sky_ptr = new Instance(spc_sky_ptr);
    instance_spc_sky_ptr->scale(1000000);
    instance_spc_sky_ptr->rotate_x(-5);
    //    instance_spc_sky_ptr->translate(0, -1000, 0);
    instance_spc_sky_ptr->set_material(sv_phong_sky_ptr);
    add_object(instance_spc_sky_ptr);



    ////////////////////////////////////////floor, Rosette///////////////////////////////////////

        // rosette parameters

    int		num_rings = 3;		// maximum of 6
    float	hole_radius = 1.0;
    float	ring_width = 1.0;
    float	rb = 0.025;			// bevel radius
    float	y0 = -0.1;			// minimum y value
    float 	y1 = 0.1;			// minimum y value


    Rosette* rosette_floor_ptr = new Rosette(num_rings, hole_radius, ring_width, rb, y0, y1);

    // put a different random marble texture on each wedge

    // ramp image:

    Image* image_floor_ptr = new Image;
    img = g_resource_path + std::string("/TextureFiles/ppm/BlueMarbleRamp.ppm");
    image_floor_ptr->read_ppm_file(img.c_str());

    // marble texture parameters

    int floor_num_octaves = 4;
    float floor_lacunarity = 2.0;
    float floor_gain = 0.5;
    float floor_perturbation = 3.0;

    int floor_num_objects = rosette_floor_ptr->get_num_objects();

    for (int j = 0; j < floor_num_objects; j++) {

        // noise:

        CubicNoise* noise_floor_ptr = new CubicNoise;
        noise_floor_ptr->set_num_octaves(floor_num_octaves);
        noise_floor_ptr->set_gain(floor_gain);			// not relevant when num_octaves = 1
        noise_floor_ptr->set_lacunarity(floor_lacunarity);     // not relevant when num_octaves = 1


        // marble texture:

        FBmTextureRamp* marble_floor_ptr = new FBmTextureRamp(image_floor_ptr);
        marble_floor_ptr->set_noise(noise_floor_ptr);
        marble_floor_ptr->set_perturbation(floor_perturbation);


        // transformed marble texture

        InstanceTexture* wedge_marble_floor_ptr = new InstanceTexture(marble_floor_ptr);
        set_rand_seed(j * 10);
        wedge_marble_floor_ptr->scale(0.25);
        wedge_marble_floor_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_floor_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_floor_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_floor_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));

        // material:

        SV_Phong* sv_phong_floor_ptr = new SV_Phong;
        sv_phong_floor_ptr->set_ka(0.25);
        sv_phong_floor_ptr->set_kd(0.75);
        sv_phong_floor_ptr->set_cd(wedge_marble_floor_ptr);
        sv_phong_floor_ptr->set_ks(0.1);
        sv_phong_floor_ptr->set_exp(20.0);

        rosette_floor_ptr->store_material(sv_phong_floor_ptr, j);	// store material in the specified wedge
    }

    rosette_floor_ptr->setup_cells();
    add_object(rosette_floor_ptr);



    ////////////////////////////////////////platform, beveled box and rectangle//////////////////////////

        // noise:

    CubicNoise* noise_bevel_b_platform_ptr = new CubicNoise;
    noise_bevel_b_platform_ptr->set_num_octaves(6);
    noise_bevel_b_platform_ptr->set_gain(2);			// not relevant when num_octaves = 1
    noise_bevel_b_platform_ptr->set_lacunarity(0.5);     // not relevant when num_octaves = 1

    // ramp image:

    Image* image_bevel_b_platform_ptr = new Image;
    img = g_resource_path + std::string("/TextureFiles/ppm/sandstone_ramp4.ppm");
    image_bevel_b_platform_ptr->read_ppm_file(img.c_str());

    // marble texture:

    FBmTextureRamp* sandstone_bevel_b_platform_ptr = new FBmTextureRamp(image_bevel_b_platform_ptr);
    sandstone_bevel_b_platform_ptr->set_noise(noise_bevel_b_platform_ptr);
    sandstone_bevel_b_platform_ptr->set_perturbation(0.1);


    // transformed sandstone texture

    InstanceTexture* wedge_sandstone_bevel_b_platform_ptr = new InstanceTexture(sandstone_bevel_b_platform_ptr);
    set_rand_seed(20);
    wedge_sandstone_bevel_b_platform_ptr->scale(0.25);
    wedge_sandstone_bevel_b_platform_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
    wedge_sandstone_bevel_b_platform_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
    wedge_sandstone_bevel_b_platform_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
    wedge_sandstone_bevel_b_platform_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));


    // material:

    SV_Phong* sv_phong_bevel_b_platform_ptr = new SV_Phong;
    sv_phong_bevel_b_platform_ptr->set_ka(0.25);
    sv_phong_bevel_b_platform_ptr->set_kd(0.75);
    sv_phong_bevel_b_platform_ptr->set_cd(wedge_sandstone_bevel_b_platform_ptr);
    sv_phong_bevel_b_platform_ptr->set_ks(0.1);
    sv_phong_bevel_b_platform_ptr->set_exp(20.0);

    BeveledBox* bevel_b_platform_ptr1 = new BeveledBox(Point3D(-2.9, 0.1, -1.9), Point3D(-2.7, 0.3, 1.9), 0.1);
    bevel_b_platform_ptr1->set_material(sv_phong_bevel_b_platform_ptr);
    add_object(bevel_b_platform_ptr1);
    BeveledBox* bevel_b_platform_ptr2 = new BeveledBox(Point3D(2.7, 0.1, -1.9), Point3D(2.9, 0.3, 1.9), 0.1);
    bevel_b_platform_ptr2->set_material(sv_phong_bevel_b_platform_ptr);
    add_object(bevel_b_platform_ptr2);
    BeveledBox* bevel_b_platform_ptr3 = new BeveledBox(Point3D(-2.7, 0.1, 1.7), Point3D(2.7, 0.3, 1.9), 0.1);
    bevel_b_platform_ptr3->set_material(sv_phong_bevel_b_platform_ptr);
    add_object(bevel_b_platform_ptr3);
    BeveledBox* bevel_b_platform_ptr4 = new BeveledBox(Point3D(-2.7, 0.1, -1.9), Point3D(2.7, 0.3, -1.7), 0.1);
    bevel_b_platform_ptr4->set_material(sv_phong_bevel_b_platform_ptr);
    add_object(bevel_b_platform_ptr4);


    Rectangle* rectangle_platform_ptr = new Rectangle(Point3D(-2.7, 0.2, 1.7), Vector3D(5.4, 0, 0), Vector3D(0, 0, -3.4));

    // noise:

    CubicNoise* noise_rectangle_platform_ptr = new CubicNoise;
    noise_rectangle_platform_ptr->set_num_octaves(8);
    noise_rectangle_platform_ptr->set_gain(0.5);
    noise_rectangle_platform_ptr->set_lacunarity(8.0);

    FBmTextureWrapped* texture_rectangle_platform_ptr = new FBmTextureWrapped(noise_rectangle_platform_ptr);
    texture_rectangle_platform_ptr->set_expansion_number(8.0);
    texture_rectangle_platform_ptr->set_color(0.7, 1.0, 0.5);   // light green
    texture_rectangle_platform_ptr->set_min_value(0.0);
    texture_rectangle_platform_ptr->set_max_value(1.0);

    // textured material:

    float exp_rectangle_platform = 1000.0;
    SV_GlossyReflector* sv_glossy_rectangle_platform_ptr = new SV_GlossyReflector;
    sv_glossy_rectangle_platform_ptr->set_samples(num_samples, exp_rectangle_platform);
    sv_glossy_rectangle_platform_ptr->set_ka(0.1);
    sv_glossy_rectangle_platform_ptr->set_kd(0.25);
    sv_glossy_rectangle_platform_ptr->set_ks(0.1);
    sv_glossy_rectangle_platform_ptr->set_exp(exp_rectangle_platform);
    sv_glossy_rectangle_platform_ptr->set_cd(texture_rectangle_platform_ptr);
    sv_glossy_rectangle_platform_ptr->set_kr(0.75);
    sv_glossy_rectangle_platform_ptr->set_exponent(exp_rectangle_platform);
    sv_glossy_rectangle_platform_ptr->set_cr(new ConstantColor(white));

    Instance* instance_sc_rectangle_platform_ptr = new Instance(rectangle_platform_ptr);
    instance_sc_rectangle_platform_ptr->set_material(sv_glossy_rectangle_platform_ptr);
    add_object(instance_sc_rectangle_platform_ptr);


    ////////////////////////////////////////steps and roof, Wedge///////////////////////////////////////

    double angle_start_s = 0;
    double angle_end_s = 360;
    int num_wedges = 9;
    double y0_s = -0.1;
    double y1_s = 0.3;
    double rb_s = 0.025;
    double y0_r = 3.7;
    double y1_r = 4.1;
    double r0_s = 5.0;
    double r1_s = 7.0;

    for (int sj = 0; sj < num_wedges; sj++) {
        double angle_width = (angle_end_s - angle_start_s) / num_wedges;  // the azimuth angle extent of each wedge
        double phi0_s = angle_start_s + sj * angle_width;
        double phi1_s = phi0_s + angle_width;
        BeveledWedge* wedge_steps_ptr = new BeveledWedge(y0_s, y1_s, r0_s, r1_s, rb_s, phi0_s, phi1_s);
        BeveledWedge* wedge_roof_ptr = new BeveledWedge(y0_r, y1_r, r0_s, r1_s, rb_s, phi0_s, phi1_s);

        // noise:

        CubicNoise* noise_step_ptr = new CubicNoise;
        noise_step_ptr->set_num_octaves(6);
        noise_step_ptr->set_gain(2);			// not relevant when num_octaves = 1
        noise_step_ptr->set_lacunarity(0.5);     // not relevant when num_octaves = 1

        // ramp image:

        Image* image_step_ptr = new Image;
        img = g_resource_path + std::string("/TextureFiles/ppm/sandstone_ramp4.ppm");
        image_step_ptr->read_ppm_file(img.c_str());

        // marble texture:

        FBmTextureRamp* sandstone_step_ptr = new FBmTextureRamp(image_step_ptr);
        sandstone_step_ptr->set_noise(noise_step_ptr);
        sandstone_step_ptr->set_perturbation(0.1);


        // transformed sandstone texture

        InstanceTexture* wedge_sandstone_step_ptr = new InstanceTexture(sandstone_step_ptr);
        set_rand_seed(20);
        wedge_sandstone_step_ptr->scale(0.25);
        wedge_sandstone_step_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
        wedge_sandstone_step_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
        wedge_sandstone_step_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
        wedge_sandstone_step_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));


        // material:

        SV_Phong* sv_phong_step_ptr = new SV_Phong;
        sv_phong_step_ptr->set_ka(0.25);
        sv_phong_step_ptr->set_kd(0.75);
        sv_phong_step_ptr->set_cd(wedge_sandstone_step_ptr);
        sv_phong_step_ptr->set_ks(0.1);
        sv_phong_step_ptr->set_exp(20.0);

        wedge_steps_ptr->set_material(sv_phong_step_ptr);
        add_object(wedge_steps_ptr);

        wedge_roof_ptr->set_material(sv_phong_step_ptr);
        add_object(wedge_roof_ptr);
    }


    ////////////////////////////////////////pillar, Rosette///////////////////////////////////////

        // rosette parameters

    int		num_rings_p = 1;		// maximum of 6
    float	hole_radius_p = 0.05;
    float	ring_width_p = 0.25;
    float	rb_p = 0.02;			// bevel radius
    float	y0_p = 0.3;			// minimum y value
    float 	y1_p = 3.7;			// minimum y value


    int pillar_num = 4;
    double pillar_angle_start = -45;
    double pillar_angle_end = 315;
    double pillar_angle_width = (pillar_angle_end - pillar_angle_start) / pillar_num;

    int rosette_num_pillar = 20;
    double rosette_height = (y1_p - y0_p) / rosette_num_pillar;

    for (int pi = 0; pi <= pillar_num; pi++) {
        double phi0_pillar = (pillar_angle_start + pillar_angle_width * pi) * PI_ON_180;

        double pillar_center_x = -(hole_radius_p + ring_width_p + r0_s) * sin((phi0_pillar - 1e-6));
        double pillar_center_z = (hole_radius_p + ring_width_p + r0_s) * cos((phi0_pillar - 1e-6));

        for (int ri = 0; ri < rosette_num_pillar; ri++) {
            double rosette_y0_temp = y0_p + ri * rosette_height;
            double rosette_y1_temp = rosette_y0_temp + rosette_height;

            Rosette* rosette_pillar_ptr = new Rosette(num_rings_p, hole_radius_p, ring_width_p, rb_p, rosette_y0_temp, rosette_y1_temp);

            // put a different random marble texture on each wedge

            // ramp image:

            Image* image_rp_ptr = new Image;
            img = g_resource_path + std::string("/TextureFiles/ppm/sandstone_ramp3.ppm");
            image_rp_ptr->read_ppm_file(img.c_str());

            // marble texture parameters

            int num_octaves_rp = 4;
            float lacunarity_rp = 2.0;
            float gain_rp = 0.5;
            float perturbation_rp = 0.1;

            int num_objects_rp = rosette_pillar_ptr->get_num_objects();

            for (int rpj = 0; rpj < num_objects_rp; rpj++) {

                // noise:

                CubicNoise* noise_rp_ptr = new CubicNoise;
                noise_rp_ptr->set_num_octaves(num_octaves_rp);
                noise_rp_ptr->set_gain(gain_rp);			// not relevant when num_octaves = 1
                noise_rp_ptr->set_lacunarity(lacunarity_rp);     // not relevant when num_octaves = 1


                // marble texture:

                FBmTextureRamp* sandstone_rp_ptr = new FBmTextureRamp(image_rp_ptr);
                sandstone_rp_ptr->set_noise(noise_rp_ptr);
                sandstone_rp_ptr->set_perturbation(perturbation_rp);


                // transformed marble texture

                InstanceTexture* wedge_sandstone_rp_ptr = new InstanceTexture(sandstone_rp_ptr);
                set_rand_seed(rpj * 10);
                wedge_sandstone_rp_ptr->scale(0.25);
                wedge_sandstone_rp_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
                wedge_sandstone_rp_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
                wedge_sandstone_rp_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
                wedge_sandstone_rp_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));

                // material:

                SV_Phong* sv_phong_rp_ptr = new SV_Phong;
                sv_phong_rp_ptr->set_ka(0.25);
                sv_phong_rp_ptr->set_kd(0.75);
                sv_phong_rp_ptr->set_cd(wedge_sandstone_rp_ptr);
                sv_phong_rp_ptr->set_ks(0.1);
                sv_phong_rp_ptr->set_exp(20.0);

                rosette_pillar_ptr->store_material(sv_phong_rp_ptr, rpj);	// store material in the specified wedge
            }

            rosette_pillar_ptr->setup_cells();

            Instance* instance_rp_ptr = new Instance(rosette_pillar_ptr);
            instance_rp_ptr->translate(pillar_center_x, 0, pillar_center_z);
            add_object(instance_rp_ptr);
        }
    }



    ////////////////////////////////////////door and wall, Wedge///////////////////////////////////////

    double _wall_width = 0.5;
    double _frame_width = 0.1;
    double _window_width = 0.8;
    double _door_width = 0.95;
    double _gap_width = 0.1;
    double unit = 5.0; // (wall+frame+window+frame+door)*2+gap


    double _wall_width_rate = _wall_width / unit;
    double _frame_width_rate = _frame_width / unit;
    double _window_width_rate = _window_width / unit;
    double _door_width_rate = _door_width / unit;
    double _gap_width_rate = _gap_width / unit;

    int unit_num = 10;
    double angle_start = 0; // -90,270
    double angle_end = 360;
    double unit_angle_width = (angle_end - angle_start) / unit_num;

    double _wall_angle_width = _wall_width_rate * unit_angle_width;
    double _frame_angle_width = _frame_width_rate * unit_angle_width;
    double _window_angle_width = _window_width_rate * unit_angle_width;
    double _door_angle_width = _door_width_rate * unit_angle_width;
    double _gap_angle_width = _gap_width_rate * unit_angle_width;

    double y10_unit = 0.3;          // bottom of the unit
    double y11_unit = 3.7;          // top of the unit
    double y20_unit = 0.3;          // the height of frame is 0.1
    double y21_unit = 0.4;
    double y30_unit = 0.4;          // the height of window is 1
    double y31_unit = 1.4;
    double y40_unit = 1.4;
    double y41_unit = 1.5;
    double y50_unit = 1.5;
    double y51_unit = 2.5;
    double y60_unit = 2.5;
    double y61_unit = 2.6;
    double y70_unit = 2.6;
    double y71_unit = 3.6;
    double y80_unit = 3.6;
    double y81_unit = 3.7;

    double r10_unit = 6.9;          // inner radius for phong material objects: wall, frame
    double r11_unit = 7.0;          // outer radius for phong material objects: wall, frame
    double r20_unit = 6.94;         // inner radius for glossy material objects: door, window
    double r21_unit = 6.96;         // outer radius for glossy material objects: door, window
    double rb1_unit = 0.01;        // beveled radius for phong material objects
    double rb2_unit = 0.005;       // beveled radius for glossy material objects

//---------------------------------------------wall material-----------------------------------

    // noise:

    CubicNoise* noise_wall_ptr1 = new CubicNoise;
    noise_wall_ptr1->set_num_octaves(6);
    noise_wall_ptr1->set_gain(0.5);			// not relevant when num_octaves = 1
    noise_wall_ptr1->set_lacunarity(2.0);     // not relevant when num_octaves = 1

    // ramp image:

    Image* image_wall_ptr1 = new Image;
    img = g_resource_path + std::string("/TextureFiles/ppm/sandstone_ramp1.ppm");
    image_wall_ptr1->read_ppm_file(img.c_str());

    // marble texture:

    FBmTextureRamp* marble_wall_ptr1 = new FBmTextureRamp(image_wall_ptr1);
    marble_wall_ptr1->set_noise(noise_wall_ptr1);
    marble_wall_ptr1->set_perturbation(0.2);

    InstanceTexture* it_wall_ptr1 = new InstanceTexture(marble_wall_ptr1);
    it_wall_ptr1->scale(0.5);
    it_wall_ptr1->rotate_z(110);
    it_wall_ptr1->translate(1.0, 4.0, 0.0);

    // material:

    SV_Phong* sv_phong_wall_ptr1 = new SV_Phong;
    sv_phong_wall_ptr1->set_ka(0.25);
    sv_phong_wall_ptr1->set_kd(0.75);
    sv_phong_wall_ptr1->set_cd(it_wall_ptr1);
    sv_phong_wall_ptr1->set_ks(0.1);
    sv_phong_wall_ptr1->set_exp(20.0);

    //---------------------------------------------vertical frame material-----------------------------------

        // wood texture

    RGBColor frame_light_color1(0.8274, 0.5059, 0.2392);
    RGBColor frame_dark_color1(0.05);

    Wood* wood_frame_ptr1 = new Wood(frame_light_color1, frame_dark_color1);
    wood_frame_ptr1->set_grainy(1.0);
    wood_frame_ptr1->set_ringy(1.0);

    InstanceTexture* transformed_wood_frame_ptr1 = new InstanceTexture(wood_frame_ptr1);
    set_rand_seed(40);
    transformed_wood_frame_ptr1->scale(0.4);
    transformed_wood_frame_ptr1->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
    transformed_wood_frame_ptr1->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
    transformed_wood_frame_ptr1->rotate_z(45.0 * (2.0 * rand_float() - 1.0));

    // material:

    SV_Phong* sv_phong_frame_ptr1 = new SV_Phong;
    sv_phong_frame_ptr1->set_ka(0.25);
    sv_phong_frame_ptr1->set_kd(0.75);
    sv_phong_frame_ptr1->set_cd(transformed_wood_frame_ptr1);
    sv_phong_frame_ptr1->set_ks(0.1);
    sv_phong_frame_ptr1->set_exp(20.0);

    //---------------------------------------------horizontal frame material-----------------------------------

        // wood texture

    RGBColor frame_light_color2(0.8274, 0.5059, 0.2392);
    RGBColor frame_dark_color2(0.05);

    Wood* wood_frame_ptr2 = new Wood(frame_light_color2, frame_dark_color2);
    wood_frame_ptr2->set_grainy(1.0);
    wood_frame_ptr2->set_ringy(1.0);

    InstanceTexture* transformed_wood_frame_ptr2 = new InstanceTexture(wood_frame_ptr2);
    set_rand_seed(80);
    transformed_wood_frame_ptr2->scale(0.8);
    transformed_wood_frame_ptr2->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
    transformed_wood_frame_ptr2->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
    transformed_wood_frame_ptr2->rotate_z(45.0 * (2.0 * rand_float() - 1.0));

    // material:

    SV_Phong* sv_phong_frame_ptr2 = new SV_Phong;
    sv_phong_frame_ptr2->set_ka(0.25);
    sv_phong_frame_ptr2->set_kd(0.75);
    sv_phong_frame_ptr2->set_cd(transformed_wood_frame_ptr2);
    sv_phong_frame_ptr2->set_ks(0.1);
    sv_phong_frame_ptr2->set_exp(20.0);

    //---------------------------------------------window material-----------------------------------

        // textured material:

    float exp_window = 100000.0; // 10000
    SV_GlossyReflector* sv_glossy_window_ptr = new SV_GlossyReflector;
    sv_glossy_window_ptr->set_samples(num_samples, exp_window);
    sv_glossy_window_ptr->set_ka(0.1);
    sv_glossy_window_ptr->set_kd(0.25);
    sv_glossy_window_ptr->set_ks(0.1);
    sv_glossy_window_ptr->set_exp(exp_window);
    sv_glossy_window_ptr->set_cd(new ConstantColor(RGBColor(0.5)));
    sv_glossy_window_ptr->set_kr(0.75);
    sv_glossy_window_ptr->set_exponent(exp_window);
    sv_glossy_window_ptr->set_cr(new ConstantColor(white));


    for (int ui = 0; ui < unit_num; ui++) {
        double phi0_unit = angle_start + unit_angle_width * ui;
        double phi1_unit = angle_start + unit_angle_width * (ui + 1);

        double phi0_wall1 = phi0_unit;
        double phi1_wall1 = phi0_wall1 + _wall_angle_width;

        BeveledWedge* wedge_wall_ptr1 = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_wall1, phi1_wall1);
        wedge_wall_ptr1->set_material(sv_phong_wall_ptr1);
        add_object(wedge_wall_ptr1);


        double phi0_frame11 = phi1_wall1;
        double phi1_frame11 = phi1_wall1 + _frame_angle_width;

        BeveledWedge* wedge_frame11_ptr = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_frame11, phi1_frame11);
        wedge_frame11_ptr->set_material(sv_phong_frame_ptr1);
        add_object(wedge_frame11_ptr);


        double phi0_frame21 = phi1_frame11;
        double phi1_frame21 = phi0_frame21 + _window_angle_width;

        BeveledWedge* wedge_frame21_ptr1 = new BeveledWedge(y20_unit, y21_unit, r10_unit, r11_unit, rb1_unit, phi0_frame21, phi1_frame21);
        wedge_frame21_ptr1->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame21_ptr1);
        BeveledWedge* wedge_frame21_ptr2 = new BeveledWedge(y40_unit, y41_unit, r10_unit, r11_unit, rb1_unit, phi0_frame21, phi1_frame21);
        wedge_frame21_ptr2->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame21_ptr2);
        BeveledWedge* wedge_frame21_ptr3 = new BeveledWedge(y60_unit, y61_unit, r10_unit, r11_unit, rb1_unit, phi0_frame21, phi1_frame21);
        wedge_frame21_ptr3->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame21_ptr3);
        BeveledWedge* wedge_frame21_ptr4 = new BeveledWedge(y80_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame21, phi1_frame21);
        wedge_frame21_ptr4->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame21_ptr4);

        BeveledWedge* wedge_window_ptr1 = new BeveledWedge(y30_unit, y31_unit, r20_unit, r21_unit, rb2_unit, phi0_frame21, phi1_frame21);
        wedge_window_ptr1->set_material(sv_glossy_window_ptr);
        add_object(wedge_window_ptr1);
        BeveledWedge* wedge_window_ptr2 = new BeveledWedge(y50_unit, y51_unit, r20_unit, r21_unit, rb2_unit, phi0_frame21, phi1_frame21);
        wedge_window_ptr2->set_material(sv_glossy_window_ptr);
        add_object(wedge_window_ptr2);
        BeveledWedge* wedge_window_ptr3 = new BeveledWedge(y70_unit, y71_unit, r20_unit, r21_unit, rb2_unit, phi0_frame21, phi1_frame21);
        wedge_window_ptr3->set_material(sv_glossy_window_ptr);
        add_object(wedge_window_ptr3);


        double phi0_frame12 = phi1_frame21;
        double phi1_frame12 = phi0_frame12 + _frame_angle_width;

        BeveledWedge* wedge_frame12_ptr = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_frame12, phi1_frame12);
        wedge_frame12_ptr->set_material(sv_phong_frame_ptr1);
        add_object(wedge_frame12_ptr);


        double phi0_frame22 = phi1_frame12;
        double phi1_frame22 = phi0_frame22 + _door_angle_width;

        BeveledWedge* wedge_frame22_ptr1 = new BeveledWedge(y20_unit, y21_unit, r10_unit, r11_unit, rb1_unit, phi0_frame22, phi1_frame22);
        wedge_frame22_ptr1->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame22_ptr1);
        BeveledWedge* wedge_frame22_ptr2 = new BeveledWedge(y60_unit, y61_unit, r10_unit, r11_unit, rb1_unit, phi0_frame22, phi1_frame22);
        wedge_frame22_ptr2->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame22_ptr2);
        BeveledWedge* wedge_frame22_ptr3 = new BeveledWedge(y80_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame22, phi1_frame22);
        wedge_frame22_ptr3->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame22_ptr3);

        BeveledWedge* wedge_door_ptr1 = new BeveledWedge(y30_unit, y51_unit, r20_unit, r21_unit, rb2_unit, phi0_frame22, phi1_frame22);
        wedge_door_ptr1->set_material(sv_glossy_window_ptr);
        add_object(wedge_door_ptr1);
        BeveledWedge* wedge_window2_ptr1 = new BeveledWedge(y70_unit, y71_unit, r20_unit, r21_unit, rb2_unit, phi0_frame22, phi1_frame22);
        wedge_window2_ptr1->set_material(sv_glossy_window_ptr);
        add_object(wedge_window2_ptr1);


        double phi0_frame13 = phi1_frame22;
        double phi1_frame13 = phi0_frame13 + _gap_angle_width;
        BeveledWedge* wedge_frame13_ptr = new BeveledWedge(y60_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame13, phi1_frame13);
        wedge_frame13_ptr->set_material(sv_phong_frame_ptr1);
        add_object(wedge_frame13_ptr);


        double phi0_frame23 = phi1_frame13;
        double phi1_frame23 = phi0_frame23 + _door_angle_width;

        BeveledWedge* wedge_frame23_ptr1 = new BeveledWedge(y20_unit, y21_unit, r10_unit, r11_unit, rb1_unit, phi0_frame23, phi1_frame23);
        wedge_frame23_ptr1->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame23_ptr1);
        BeveledWedge* wedge_frame23_ptr2 = new BeveledWedge(y60_unit, y61_unit, r10_unit, r11_unit, rb1_unit, phi0_frame23, phi1_frame23);
        wedge_frame23_ptr2->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame23_ptr2);
        BeveledWedge* wedge_frame23_ptr3 = new BeveledWedge(y80_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame23, phi1_frame23);
        wedge_frame23_ptr3->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame23_ptr3);

        BeveledWedge* wedge_door_ptr2 = new BeveledWedge(y30_unit, y51_unit, r20_unit, r21_unit, rb2_unit, phi0_frame23, phi1_frame23);
        wedge_door_ptr2->set_material(sv_glossy_window_ptr);
        add_object(wedge_door_ptr2);
        BeveledWedge* wedge_window3_ptr1 = new BeveledWedge(y70_unit, y71_unit, r20_unit, r21_unit, rb2_unit, phi0_frame23, phi1_frame23);
        wedge_window3_ptr1->set_material(sv_glossy_window_ptr);
        add_object(wedge_window3_ptr1);


        double phi0_frame14 = phi1_frame23;
        double phi1_frame14 = phi0_frame14 + _frame_angle_width;

        BeveledWedge* wedge_frame14_ptr = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_frame14, phi1_frame14);
        wedge_frame14_ptr->set_material(sv_phong_frame_ptr1);
        add_object(wedge_frame14_ptr);


        double phi0_frame24 = phi1_frame14;
        double phi1_frame24 = phi0_frame24 + _window_angle_width;

        BeveledWedge* wedge_frame24_ptr1 = new BeveledWedge(y20_unit, y21_unit, r10_unit, r11_unit, rb1_unit, phi0_frame24, phi1_frame24);
        wedge_frame24_ptr1->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame24_ptr1);
        BeveledWedge* wedge_frame24_ptr2 = new BeveledWedge(y40_unit, y41_unit, r10_unit, r11_unit, rb1_unit, phi0_frame24, phi1_frame24);
        wedge_frame24_ptr2->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame24_ptr2);
        BeveledWedge* wedge_frame24_ptr3 = new BeveledWedge(y60_unit, y61_unit, r10_unit, r11_unit, rb1_unit, phi0_frame24, phi1_frame24);
        wedge_frame24_ptr3->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame24_ptr3);
        BeveledWedge* wedge_frame24_ptr4 = new BeveledWedge(y80_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame24, phi1_frame24);
        wedge_frame24_ptr4->set_material(sv_phong_frame_ptr2);
        add_object(wedge_frame24_ptr4);

        BeveledWedge* wedge_window4_ptr1 = new BeveledWedge(y30_unit, y31_unit, r20_unit, r21_unit, rb2_unit, phi0_frame24, phi1_frame24);
        wedge_window4_ptr1->set_material(sv_glossy_window_ptr);
        add_object(wedge_window4_ptr1);
        BeveledWedge* wedge_window4_ptr2 = new BeveledWedge(y50_unit, y51_unit, r20_unit, r21_unit, rb2_unit, phi0_frame24, phi1_frame24);
        wedge_window4_ptr2->set_material(sv_glossy_window_ptr);
        add_object(wedge_window4_ptr2);
        BeveledWedge* wedge_window4_ptr3 = new BeveledWedge(y70_unit, y71_unit, r20_unit, r21_unit, rb2_unit, phi0_frame24, phi1_frame24);
        wedge_window4_ptr3->set_material(sv_glossy_window_ptr);
        add_object(wedge_window4_ptr3);


        double phi0_frame15 = phi1_frame24;
        double phi1_frame15 = phi0_frame15 + _frame_angle_width;

        BeveledWedge* wedge_frame15_ptr = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_frame15, phi1_frame15);
        wedge_frame15_ptr->set_material(sv_phong_frame_ptr1);
        add_object(wedge_frame15_ptr);


        double phi0_wall2 = phi1_frame15;
        double phi1_wall2 = phi1_unit;

        BeveledWedge* wedge_wall_ptr2 = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_wall2, phi1_wall2);
        wedge_wall_ptr2->set_material(sv_phong_wall_ptr1);
        add_object(wedge_wall_ptr2);
    }


    ////////////////////////////////////////dragon////////////////////////////////////////////////

    auto dragon = g_resource_path + std::string("/PLYFiles/dragon.ply");

    const char* file_dragon_name = dragon.c_str();

    Grid* grid_dragon_ptr = new Grid(new Mesh);
    grid_dragon_ptr->read_smooth_triangles(file_dragon_name);
    grid_dragon_ptr->setup_cells();

    // noise:

    CubicNoise* noise_dragon_ptr = new CubicNoise;
    noise_dragon_ptr->set_num_octaves(6);
    noise_dragon_ptr->set_gain(0.5);
    noise_dragon_ptr->set_lacunarity(2.0);

    // ramp image:

    Image* image_dragon_ptr = new Image;
    img = g_resource_path + std::string("/TextureFiles/ppm/BlueMarbleRamp.ppm");
    image_dragon_ptr->read_ppm_file(img.c_str());

    // marble texture:

    FBmTextureRamp* marble_dragon_ptr = new FBmTextureRamp(image_dragon_ptr);
    marble_dragon_ptr->set_noise(noise_dragon_ptr);
    marble_dragon_ptr->set_perturbation(4.0);

    InstanceTexture* it_dragon_ptr = new InstanceTexture(marble_dragon_ptr);
    it_dragon_ptr->scale(0.05);
    //	it_dragon_ptr->rotate_z(110);
    //	it_dragon_ptr->translate(1.0, 4.0, 0.0);


        // material:

    SV_Phong* sv_phong_dragon_ptr = new SV_Phong;
    sv_phong_dragon_ptr->set_ka(0.25);
    sv_phong_dragon_ptr->set_kd(0.75);
    sv_phong_dragon_ptr->set_cd(it_dragon_ptr);
    sv_phong_dragon_ptr->set_ks(0.1);
    sv_phong_dragon_ptr->set_exp(20.0);


    Instance* instance_dragon_ptr = new Instance(grid_dragon_ptr);
    instance_dragon_ptr->scale(20, 10, 20);
    instance_dragon_ptr->translate(0, -0.3, 0);
    instance_dragon_ptr->set_material(sv_phong_dragon_ptr);

    add_object(instance_dragon_ptr);
}
#endif // TYPE

#if TYPE == 4
void
World::build(void) {
    int num_samples = 1;
    int a = 5;

    vp.set_hres(1000 / a);
    vp.set_vres(500 / a);
    vp.set_samples(num_samples);

    tracer_ptr = new Whitted(this);

    Pinhole* pinhole_ptr = new Pinhole;
    pinhole_ptr->set_eye(0.0, 2.6, 6.8);
    pinhole_ptr->set_lookat(0.0, 0.0, -8);
    pinhole_ptr->set_view_distance(200 / a);
    pinhole_ptr->compute_uvw();
    set_camera(pinhole_ptr);

    PointLight* point_light_ptr1 = new PointLight;
    point_light_ptr1->set_location(0, 3, -5);
    point_light_ptr1->scale_radiance(2.0);
    point_light_ptr1->set_cast_shadow(true);
    add_light(point_light_ptr1);

    PointLight* point_light_ptr2 = new PointLight;
    point_light_ptr2->set_location(0, 3, 5);
    point_light_ptr2->scale_radiance(2.0);
    point_light_ptr2->set_cast_shadow(true);
    add_light(point_light_ptr2);


    Grid* scene_grid_ptr = new Grid;

    ////////////////////////////////////////sky, SpherePartConcave///////////////////////////////////////

    SpherePartConcave* spc_sky_ptr = new SpherePartConcave(Point3D(0, 0, 0), 1, 0, 360, 0, 90);

    // image:

    Image* image_sky_ptr = new Image;
    //	image_sky_ptr->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/MorningSky.ppm");
    image_sky_ptr->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/EveningSky.ppm");

    // image based texture:

    ImageTexture* texture_image_sky_ptr = new ImageTexture;
    texture_image_sky_ptr->set_image(image_sky_ptr);

    InstanceTexture* it_image_sky_ptr = new InstanceTexture(texture_image_sky_ptr);
    it_image_sky_ptr->scale(1000000);

    // material:

    SV_Phong* sv_phong_sky_ptr = new SV_Phong;
    sv_phong_sky_ptr->set_ka(0.1);
    sv_phong_sky_ptr->set_kd(0.25);
    sv_phong_sky_ptr->set_cd(texture_image_sky_ptr);
    sv_phong_sky_ptr->set_ks(0.1);
    sv_phong_sky_ptr->set_exp(20.0);


    Instance* instance_spc_sky_ptr = new Instance(spc_sky_ptr);
    instance_spc_sky_ptr->scale(1000000);
    instance_spc_sky_ptr->rotate_x(-5);
    //    instance_spc_sky_ptr->translate(0, -1000, 0);
    instance_spc_sky_ptr->set_material(sv_phong_sky_ptr);
    add_object(instance_spc_sky_ptr);



    ////////////////////////////////////////floor, Rosette///////////////////////////////////////

        // rosette parameters

    int		num_rings = 3;		// maximum of 6
    float	hole_radius = 1.0;
    float	ring_width = 1.0;
    float	rb = 0.025;			// bevel radius
    float	y0 = -0.1;			// minimum y value
    float 	y1 = 0.1;			// minimum y value


    Rosette* rosette_floor_ptr = new Rosette(num_rings, hole_radius, ring_width, rb, y0, y1);

    // put a different random marble texture on each wedge

    // ramp image:

    Image* image_floor_ptr = new Image;
    image_floor_ptr->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/BlueMarbleRamp.ppm");

    // marble texture parameters

    int floor_num_octaves = 4;
    float floor_lacunarity = 2.0;
    float floor_gain = 0.5;
    float floor_perturbation = 3.0;

    int floor_num_objects = rosette_floor_ptr->get_num_objects();

    for (int j = 0; j < floor_num_objects; j++) {

        // noise:

        CubicNoise* noise_floor_ptr = new CubicNoise;
        noise_floor_ptr->set_num_octaves(floor_num_octaves);
        noise_floor_ptr->set_gain(floor_gain);			// not relevant when num_octaves = 1
        noise_floor_ptr->set_lacunarity(floor_lacunarity);     // not relevant when num_octaves = 1


        // marble texture:

        FBmTextureRamp* marble_floor_ptr = new FBmTextureRamp(image_floor_ptr);
        marble_floor_ptr->set_noise(noise_floor_ptr);
        marble_floor_ptr->set_perturbation(floor_perturbation);


        // transformed marble texture

        InstanceTexture* wedge_marble_floor_ptr = new InstanceTexture(marble_floor_ptr);
        set_rand_seed(j * 10);
        wedge_marble_floor_ptr->scale(0.25);
        wedge_marble_floor_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_floor_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_floor_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
        wedge_marble_floor_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));

        // material:

        SV_Phong* sv_phong_floor_ptr = new SV_Phong;
        sv_phong_floor_ptr->set_ka(0.25);
        sv_phong_floor_ptr->set_kd(0.75);
        sv_phong_floor_ptr->set_cd(wedge_marble_floor_ptr);
        sv_phong_floor_ptr->set_ks(0.1);
        sv_phong_floor_ptr->set_exp(20.0);

        rosette_floor_ptr->store_material(sv_phong_floor_ptr, j);	// store material in the specified wedge
    }

    rosette_floor_ptr->setup_cells();
    scene_grid_ptr->add_object(rosette_floor_ptr);



    ////////////////////////////////////////platform, beveled box and rectangle//////////////////////////

    Grid* platform_grid_ptr = new Grid;

    // noise:

    CubicNoise* noise_bevel_b_platform_ptr = new CubicNoise;
    noise_bevel_b_platform_ptr->set_num_octaves(6);
    noise_bevel_b_platform_ptr->set_gain(2);			// not relevant when num_octaves = 1
    noise_bevel_b_platform_ptr->set_lacunarity(0.5);     // not relevant when num_octaves = 1

    // ramp image:

    Image* image_bevel_b_platform_ptr = new Image;
    image_bevel_b_platform_ptr->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/sandstone_ramp4.ppm");

    // marble texture:

    FBmTextureRamp* sandstone_bevel_b_platform_ptr = new FBmTextureRamp(image_bevel_b_platform_ptr);
    sandstone_bevel_b_platform_ptr->set_noise(noise_bevel_b_platform_ptr);
    sandstone_bevel_b_platform_ptr->set_perturbation(0.1);


    // transformed sandstone texture

    InstanceTexture* wedge_sandstone_bevel_b_platform_ptr = new InstanceTexture(sandstone_bevel_b_platform_ptr);
    set_rand_seed(20);
    wedge_sandstone_bevel_b_platform_ptr->scale(0.25);
    wedge_sandstone_bevel_b_platform_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
    wedge_sandstone_bevel_b_platform_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
    wedge_sandstone_bevel_b_platform_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
    wedge_sandstone_bevel_b_platform_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));


    // material:

    SV_Phong* sv_phong_bevel_b_platform_ptr = new SV_Phong;
    sv_phong_bevel_b_platform_ptr->set_ka(0.25);
    sv_phong_bevel_b_platform_ptr->set_kd(0.75);
    sv_phong_bevel_b_platform_ptr->set_cd(wedge_sandstone_bevel_b_platform_ptr);
    sv_phong_bevel_b_platform_ptr->set_ks(0.1);
    sv_phong_bevel_b_platform_ptr->set_exp(20.0);

    BeveledBox* bevel_b_platform_ptr1 = new BeveledBox(Point3D(-2.9, 0.1, -1.9), Point3D(-2.7, 0.3, 1.9), 0.1);
    bevel_b_platform_ptr1->set_material(sv_phong_bevel_b_platform_ptr);
    platform_grid_ptr->add_object(bevel_b_platform_ptr1);
    BeveledBox* bevel_b_platform_ptr2 = new BeveledBox(Point3D(2.7, 0.1, -1.9), Point3D(2.9, 0.3, 1.9), 0.1);
    bevel_b_platform_ptr2->set_material(sv_phong_bevel_b_platform_ptr);
    platform_grid_ptr->add_object(bevel_b_platform_ptr2);
    BeveledBox* bevel_b_platform_ptr3 = new BeveledBox(Point3D(-2.7, 0.1, 1.7), Point3D(2.7, 0.3, 1.9), 0.1);
    bevel_b_platform_ptr3->set_material(sv_phong_bevel_b_platform_ptr);
    platform_grid_ptr->add_object(bevel_b_platform_ptr3);
    BeveledBox* bevel_b_platform_ptr4 = new BeveledBox(Point3D(-2.7, 0.1, -1.9), Point3D(2.7, 0.3, -1.7), 0.1);
    bevel_b_platform_ptr4->set_material(sv_phong_bevel_b_platform_ptr);
    platform_grid_ptr->add_object(bevel_b_platform_ptr4);


    Rectangle* rectangle_platform_ptr = new Rectangle(Point3D(-2.7, 0.2, 1.7), Vector3D(5.4, 0, 0), Vector3D(0, 0, -3.4));

    // noise:

    CubicNoise* noise_rectangle_platform_ptr = new CubicNoise;
    noise_rectangle_platform_ptr->set_num_octaves(8);
    noise_rectangle_platform_ptr->set_gain(0.5);
    noise_rectangle_platform_ptr->set_lacunarity(8.0);

    FBmTextureWrapped* texture_rectangle_platform_ptr = new FBmTextureWrapped(noise_rectangle_platform_ptr);
    texture_rectangle_platform_ptr->set_expansion_number(8.0);
    texture_rectangle_platform_ptr->set_color(0.7, 1.0, 0.5);   // light green
    texture_rectangle_platform_ptr->set_min_value(0.0);
    texture_rectangle_platform_ptr->set_max_value(1.0);

    // textured material:

    float exp_rectangle_platform = 1000.0;
    SV_GlossyReflector* sv_glossy_rectangle_platform_ptr = new SV_GlossyReflector;
    sv_glossy_rectangle_platform_ptr->set_samples(num_samples, exp_rectangle_platform);
    sv_glossy_rectangle_platform_ptr->set_ka(0.1);
    sv_glossy_rectangle_platform_ptr->set_kd(0.25);
    sv_glossy_rectangle_platform_ptr->set_ks(0.1);
    sv_glossy_rectangle_platform_ptr->set_exp(exp_rectangle_platform);
    sv_glossy_rectangle_platform_ptr->set_cd(texture_rectangle_platform_ptr);
    sv_glossy_rectangle_platform_ptr->set_kr(0.75);
    sv_glossy_rectangle_platform_ptr->set_exponent(exp_rectangle_platform);
    sv_glossy_rectangle_platform_ptr->set_cr(new ConstantColor(white));

    Instance* instance_sc_rectangle_platform_ptr = new Instance(rectangle_platform_ptr);
    instance_sc_rectangle_platform_ptr->set_material(sv_glossy_rectangle_platform_ptr);
    platform_grid_ptr->add_object(instance_sc_rectangle_platform_ptr);

    platform_grid_ptr->setup_cells();
    scene_grid_ptr->add_object(platform_grid_ptr);

    ////////////////////////////////////////steps and roof, Wedge///////////////////////////////////////
    Grid* steps_roof_grid_ptr = new Grid;

    double angle_start_s = 0;
    double angle_end_s = 360;
    int num_wedges = 9;
    double y0_s = -0.1;
    double y1_s = 0.3;
    double rb_s = 0.025;
    double y0_r = 3.7;
    double y1_r = 4.1;
    double r0_s = 5.0;
    double r1_s = 7.0;

    for (int sj = 0; sj < num_wedges; sj++) {
        double angle_width = (angle_end_s - angle_start_s) / num_wedges;  // the azimuth angle extent of each wedge
        double phi0_s = angle_start_s + sj * angle_width;
        double phi1_s = phi0_s + angle_width;
        BeveledWedge* wedge_steps_ptr = new BeveledWedge(y0_s, y1_s, r0_s, r1_s, rb_s, phi0_s, phi1_s);
        BeveledWedge* wedge_roof_ptr = new BeveledWedge(y0_r, y1_r, r0_s, r1_s, rb_s, phi0_s, phi1_s);

        // noise:

        CubicNoise* noise_step_ptr = new CubicNoise;
        noise_step_ptr->set_num_octaves(6);
        noise_step_ptr->set_gain(2);			// not relevant when num_octaves = 1
        noise_step_ptr->set_lacunarity(0.5);     // not relevant when num_octaves = 1

        // ramp image:

        Image* image_step_ptr = new Image;
        image_step_ptr->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/sandstone_ramp4.ppm");

        // marble texture:

        FBmTextureRamp* sandstone_step_ptr = new FBmTextureRamp(image_step_ptr);
        sandstone_step_ptr->set_noise(noise_step_ptr);
        sandstone_step_ptr->set_perturbation(0.1);


        // transformed sandstone texture

        InstanceTexture* wedge_sandstone_step_ptr = new InstanceTexture(sandstone_step_ptr);
        set_rand_seed(20);
        wedge_sandstone_step_ptr->scale(0.25);
        wedge_sandstone_step_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
        wedge_sandstone_step_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
        wedge_sandstone_step_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
        wedge_sandstone_step_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));


        // material:

        SV_Phong* sv_phong_step_ptr = new SV_Phong;
        sv_phong_step_ptr->set_ka(0.25);
        sv_phong_step_ptr->set_kd(0.75);
        sv_phong_step_ptr->set_cd(wedge_sandstone_step_ptr);
        sv_phong_step_ptr->set_ks(0.1);
        sv_phong_step_ptr->set_exp(20.0);

        wedge_steps_ptr->set_material(sv_phong_step_ptr);
        steps_roof_grid_ptr->add_object(wedge_steps_ptr);

        wedge_roof_ptr->set_material(sv_phong_step_ptr);
        steps_roof_grid_ptr->add_object(wedge_roof_ptr);
    }
    steps_roof_grid_ptr->setup_cells();
    scene_grid_ptr->add_object(steps_roof_grid_ptr);


    ////////////////////////////////////////pillar, Rosette///////////////////////////////////////
    Grid* pillar_grid_ptr = new Grid;
    // rosette parameters

    int		num_rings_p = 1;		// maximum of 6
    float	hole_radius_p = 0.05;
    float	ring_width_p = 0.25;
    float	rb_p = 0.02;			// bevel radius
    float	y0_p = 0.3;			// minimum y value
    float 	y1_p = 3.7;			// minimum y value


    int pillar_num = 4;
    double pillar_angle_start = -45;
    double pillar_angle_end = 315;
    double pillar_angle_width = (pillar_angle_end - pillar_angle_start) / pillar_num;

    int rosette_num_pillar = 20;
    double rosette_height = (y1_p - y0_p) / rosette_num_pillar;

    for (int pi = 0; pi <= pillar_num; pi++) {
        double phi0_pillar = (pillar_angle_start + pillar_angle_width * pi) * PI_ON_180;

        double pillar_center_x = -(hole_radius_p + ring_width_p + r0_s) * sin((phi0_pillar - 1e-6));
        double pillar_center_z = (hole_radius_p + ring_width_p + r0_s) * cos((phi0_pillar - 1e-6));

        Grid* single_pillar_grid_ptr = new Grid;

        for (int ri = 0; ri < rosette_num_pillar; ri++) {
            double rosette_y0_temp = y0_p + ri * rosette_height;
            double rosette_y1_temp = rosette_y0_temp + rosette_height;

            Rosette* rosette_pillar_ptr = new Rosette(num_rings_p, hole_radius_p, ring_width_p, rb_p, rosette_y0_temp, rosette_y1_temp);

            // put a different random marble texture on each wedge

            // ramp image:

            Image* image_rp_ptr = new Image;
            image_rp_ptr->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/sandstone_ramp3.ppm");

            // marble texture parameters

            int num_octaves_rp = 4;
            float lacunarity_rp = 2.0;
            float gain_rp = 0.5;
            float perturbation_rp = 0.1;

            int num_objects_rp = rosette_pillar_ptr->get_num_objects();

            for (int rpj = 0; rpj < num_objects_rp; rpj++) {

                // noise:

                CubicNoise* noise_rp_ptr = new CubicNoise;
                noise_rp_ptr->set_num_octaves(num_octaves_rp);
                noise_rp_ptr->set_gain(gain_rp);			// not relevant when num_octaves = 1
                noise_rp_ptr->set_lacunarity(lacunarity_rp);     // not relevant when num_octaves = 1


                // marble texture:

                FBmTextureRamp* sandstone_rp_ptr = new FBmTextureRamp(image_rp_ptr);
                sandstone_rp_ptr->set_noise(noise_rp_ptr);
                sandstone_rp_ptr->set_perturbation(perturbation_rp);


                // transformed marble texture

                InstanceTexture* wedge_sandstone_rp_ptr = new InstanceTexture(sandstone_rp_ptr);
                set_rand_seed(rpj * 10);
                wedge_sandstone_rp_ptr->scale(0.25);
                wedge_sandstone_rp_ptr->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
                wedge_sandstone_rp_ptr->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
                wedge_sandstone_rp_ptr->rotate_z(45.0 * (2.0 * rand_float() - 1.0));
                wedge_sandstone_rp_ptr->translate(10.0 * (2.0 * rand_float() - 1.0), 20.0 * (2.0 * rand_float() - 1.0), 30.0 * (2.0 * rand_float() - 1.0));

                // material:

                SV_Phong* sv_phong_rp_ptr = new SV_Phong;
                sv_phong_rp_ptr->set_ka(0.25);
                sv_phong_rp_ptr->set_kd(0.75);
                sv_phong_rp_ptr->set_cd(wedge_sandstone_rp_ptr);
                sv_phong_rp_ptr->set_ks(0.1);
                sv_phong_rp_ptr->set_exp(20.0);

                rosette_pillar_ptr->store_material(sv_phong_rp_ptr, rpj);	// store material in the specified wedge
            }

            rosette_pillar_ptr->setup_cells();

            Instance* instance_rp_ptr = new Instance(rosette_pillar_ptr);
            instance_rp_ptr->translate(pillar_center_x, 0, pillar_center_z);
            single_pillar_grid_ptr->add_object(instance_rp_ptr);
        }
        single_pillar_grid_ptr->setup_cells();
        pillar_grid_ptr->add_object(single_pillar_grid_ptr);
    }
    pillar_grid_ptr->setup_cells();
    scene_grid_ptr->add_object(pillar_grid_ptr);


    ////////////////////////////////////////door and wall, Wedge///////////////////////////////////////

    Grid* door_wall_grid_ptr = new Grid;

    double _wall_width = 0.5;
    double _frame_width = 0.1;
    double _window_width = 0.8;
    double _door_width = 0.95;
    double _gap_width = 0.1;
    double unit = 5.0; // (wall+frame+window+frame+door)*2+gap


    double _wall_width_rate = _wall_width / unit;
    double _frame_width_rate = _frame_width / unit;
    double _window_width_rate = _window_width / unit;
    double _door_width_rate = _door_width / unit;
    double _gap_width_rate = _gap_width / unit;

    int unit_num = 10;
    double angle_start = 0;
    double angle_end = 360;
    double unit_angle_width = (angle_end - angle_start) / unit_num;

    double _wall_angle_width = _wall_width_rate * unit_angle_width;
    double _frame_angle_width = _frame_width_rate * unit_angle_width;
    double _window_angle_width = _window_width_rate * unit_angle_width;
    double _door_angle_width = _door_width_rate * unit_angle_width;
    double _gap_angle_width = _gap_width_rate * unit_angle_width;

    double y10_unit = 0.3;
    double y11_unit = 3.7;
    double y20_unit = 0.3;
    double y21_unit = 0.4;
    double y30_unit = 0.4;
    double y31_unit = 1.4;
    double y40_unit = 1.4;
    double y41_unit = 1.5;
    double y50_unit = 1.5;
    double y51_unit = 2.5;
    double y60_unit = 2.5;
    double y61_unit = 2.6;
    double y70_unit = 2.6;
    double y71_unit = 3.6;
    double y80_unit = 3.6;
    double y81_unit = 3.7;

    double r10_unit = 6.9;
    double r11_unit = 7.0;
    double r20_unit = 6.94;
    double r21_unit = 6.96;
    double rb1_unit = 0.01;
    double rb2_unit = 0.005;

    //---------------------------------------------wall material-----------------------------------

        // noise:

    CubicNoise* noise_wall_ptr1 = new CubicNoise;
    noise_wall_ptr1->set_num_octaves(6);
    noise_wall_ptr1->set_gain(0.5);			// not relevant when num_octaves = 1
    noise_wall_ptr1->set_lacunarity(2.0);     // not relevant when num_octaves = 1

    // ramp image:

    Image* image_wall_ptr1 = new Image;
    //    image_wall_ptr1->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/BlueMarbleRamp.ppm");
    image_wall_ptr1->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/sandstone_ramp1.ppm");

    // marble texture:

    FBmTextureRamp* marble_wall_ptr1 = new FBmTextureRamp(image_wall_ptr1);
    marble_wall_ptr1->set_noise(noise_wall_ptr1);
    marble_wall_ptr1->set_perturbation(0.2);

    InstanceTexture* it_wall_ptr1 = new InstanceTexture(marble_wall_ptr1);
    it_wall_ptr1->scale(0.5);
    it_wall_ptr1->rotate_z(110);
    it_wall_ptr1->translate(1.0, 4.0, 0.0);

    // material:

    SV_Phong* sv_phong_wall_ptr1 = new SV_Phong;
    sv_phong_wall_ptr1->set_ka(0.25);
    sv_phong_wall_ptr1->set_kd(0.75);
    sv_phong_wall_ptr1->set_cd(it_wall_ptr1);
    sv_phong_wall_ptr1->set_ks(0.1);
    sv_phong_wall_ptr1->set_exp(20.0);

    //---------------------------------------------vertical frame material-----------------------------------

        // wood texture

    RGBColor frame_light_color1(0.8274, 0.5059, 0.2392);
    RGBColor frame_dark_color1(0.05);

    Wood* wood_frame_ptr1 = new Wood(frame_light_color1, frame_dark_color1);
    wood_frame_ptr1->set_grainy(1.0);
    wood_frame_ptr1->set_ringy(1.0);

    InstanceTexture* transformed_wood_frame_ptr1 = new InstanceTexture(wood_frame_ptr1);
    set_rand_seed(40);
    transformed_wood_frame_ptr1->scale(0.4);
    transformed_wood_frame_ptr1->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
    transformed_wood_frame_ptr1->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
    transformed_wood_frame_ptr1->rotate_z(45.0 * (2.0 * rand_float() - 1.0));

    // material:

    SV_Phong* sv_phong_frame_ptr1 = new SV_Phong;
    sv_phong_frame_ptr1->set_ka(0.25);
    sv_phong_frame_ptr1->set_kd(0.75);
    sv_phong_frame_ptr1->set_cd(transformed_wood_frame_ptr1);
    sv_phong_frame_ptr1->set_ks(0.1);
    sv_phong_frame_ptr1->set_exp(20.0);

    //---------------------------------------------horizontal frame material-----------------------------------

        // wood texture

    RGBColor frame_light_color2(0.8274, 0.5059, 0.2392);
    RGBColor frame_dark_color2(0.05);

    Wood* wood_frame_ptr2 = new Wood(frame_light_color2, frame_dark_color2);
    wood_frame_ptr2->set_grainy(1.0);
    wood_frame_ptr2->set_ringy(1.0);

    InstanceTexture* transformed_wood_frame_ptr2 = new InstanceTexture(wood_frame_ptr2);
    set_rand_seed(80);
    transformed_wood_frame_ptr2->scale(0.8);
    transformed_wood_frame_ptr2->rotate_x(20.0 * (2.0 * rand_float() - 1.0));
    transformed_wood_frame_ptr2->rotate_y(30.0 * (2.0 * rand_float() - 1.0));
    transformed_wood_frame_ptr2->rotate_z(45.0 * (2.0 * rand_float() - 1.0));

    // material:

    SV_Phong* sv_phong_frame_ptr2 = new SV_Phong;
    sv_phong_frame_ptr2->set_ka(0.25);
    sv_phong_frame_ptr2->set_kd(0.75);
    sv_phong_frame_ptr2->set_cd(transformed_wood_frame_ptr2);
    sv_phong_frame_ptr2->set_ks(0.1);
    sv_phong_frame_ptr2->set_exp(20.0);

    //---------------------------------------------window material-----------------------------------

        // textured material:

    float exp_window = 10000.0;
    SV_GlossyReflector* sv_glossy_window_ptr = new SV_GlossyReflector;
    sv_glossy_window_ptr->set_samples(num_samples, exp_window);
    sv_glossy_window_ptr->set_ka(0.1);
    sv_glossy_window_ptr->set_kd(0.25);
    sv_glossy_window_ptr->set_ks(0.1);
    sv_glossy_window_ptr->set_exp(exp_window);
    sv_glossy_window_ptr->set_cd(new ConstantColor(RGBColor(0.5)));
    sv_glossy_window_ptr->set_kr(0.75);
    sv_glossy_window_ptr->set_exponent(exp_window);
    sv_glossy_window_ptr->set_cr(new ConstantColor(white));


    for (int ui = 0; ui < unit_num; ui++) {
        double phi0_unit = angle_start + unit_angle_width * ui;
        double phi1_unit = angle_start + unit_angle_width * (ui + 1);

        double phi0_wall1 = phi0_unit;
        double phi1_wall1 = phi0_wall1 + _wall_angle_width;

        BeveledWedge* wedge_wall_ptr1 = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_wall1, phi1_wall1);
        wedge_wall_ptr1->set_material(sv_phong_wall_ptr1);
        door_wall_grid_ptr->add_object(wedge_wall_ptr1);


        double phi0_frame11 = phi1_wall1;
        double phi1_frame11 = phi1_wall1 + _frame_angle_width;

        BeveledWedge* wedge_frame11_ptr = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_frame11, phi1_frame11);
        wedge_frame11_ptr->set_material(sv_phong_frame_ptr1);
        door_wall_grid_ptr->add_object(wedge_frame11_ptr);


        double phi0_frame21 = phi1_frame11;
        double phi1_frame21 = phi0_frame21 + _window_angle_width;

        BeveledWedge* wedge_frame21_ptr1 = new BeveledWedge(y20_unit, y21_unit, r10_unit, r11_unit, rb1_unit, phi0_frame21, phi1_frame21);
        wedge_frame21_ptr1->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame21_ptr1);
        BeveledWedge* wedge_frame21_ptr2 = new BeveledWedge(y40_unit, y41_unit, r10_unit, r11_unit, rb1_unit, phi0_frame21, phi1_frame21);
        wedge_frame21_ptr2->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame21_ptr2);
        BeveledWedge* wedge_frame21_ptr3 = new BeveledWedge(y60_unit, y61_unit, r10_unit, r11_unit, rb1_unit, phi0_frame21, phi1_frame21);
        wedge_frame21_ptr3->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame21_ptr3);
        BeveledWedge* wedge_frame21_ptr4 = new BeveledWedge(y80_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame21, phi1_frame21);
        wedge_frame21_ptr4->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame21_ptr4);

        BeveledWedge* wedge_window_ptr1 = new BeveledWedge(y30_unit, y31_unit, r20_unit, r21_unit, rb2_unit, phi0_frame21, phi1_frame21);
        wedge_window_ptr1->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_window_ptr1);
        BeveledWedge* wedge_window_ptr2 = new BeveledWedge(y50_unit, y51_unit, r20_unit, r21_unit, rb2_unit, phi0_frame21, phi1_frame21);
        wedge_window_ptr2->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_window_ptr2);
        BeveledWedge* wedge_window_ptr3 = new BeveledWedge(y70_unit, y71_unit, r20_unit, r21_unit, rb2_unit, phi0_frame21, phi1_frame21);
        wedge_window_ptr3->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_window_ptr3);


        double phi0_frame12 = phi1_frame21;
        double phi1_frame12 = phi0_frame12 + _frame_angle_width;

        BeveledWedge* wedge_frame12_ptr = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_frame12, phi1_frame12);
        wedge_frame12_ptr->set_material(sv_phong_frame_ptr1);
        door_wall_grid_ptr->add_object(wedge_frame12_ptr);


        double phi0_frame22 = phi1_frame12;
        double phi1_frame22 = phi0_frame22 + _door_angle_width;

        BeveledWedge* wedge_frame22_ptr1 = new BeveledWedge(y20_unit, y21_unit, r10_unit, r11_unit, rb1_unit, phi0_frame22, phi1_frame22);
        wedge_frame22_ptr1->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame22_ptr1);
        BeveledWedge* wedge_frame22_ptr2 = new BeveledWedge(y60_unit, y61_unit, r10_unit, r11_unit, rb1_unit, phi0_frame22, phi1_frame22);
        wedge_frame22_ptr2->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame22_ptr2);
        BeveledWedge* wedge_frame22_ptr3 = new BeveledWedge(y80_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame22, phi1_frame22);
        wedge_frame22_ptr3->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame22_ptr3);

        BeveledWedge* wedge_door_ptr1 = new BeveledWedge(y30_unit, y51_unit, r20_unit, r21_unit, rb2_unit, phi0_frame22, phi1_frame22);
        wedge_door_ptr1->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_door_ptr1);
        BeveledWedge* wedge_window2_ptr1 = new BeveledWedge(y70_unit, y71_unit, r20_unit, r21_unit, rb2_unit, phi0_frame22, phi1_frame22);
        wedge_window2_ptr1->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_window2_ptr1);


        double phi0_frame13 = phi1_frame22;
        double phi1_frame13 = phi0_frame13 + _gap_angle_width;
        BeveledWedge* wedge_frame13_ptr = new BeveledWedge(y60_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame13, phi1_frame13);
        wedge_frame13_ptr->set_material(sv_phong_frame_ptr1);
        door_wall_grid_ptr->add_object(wedge_frame13_ptr);


        double phi0_frame23 = phi1_frame13;
        double phi1_frame23 = phi0_frame23 + _door_angle_width;

        BeveledWedge* wedge_frame23_ptr1 = new BeveledWedge(y20_unit, y21_unit, r10_unit, r11_unit, rb1_unit, phi0_frame23, phi1_frame23);
        wedge_frame23_ptr1->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame23_ptr1);
        BeveledWedge* wedge_frame23_ptr2 = new BeveledWedge(y60_unit, y61_unit, r10_unit, r11_unit, rb1_unit, phi0_frame23, phi1_frame23);
        wedge_frame23_ptr2->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame23_ptr2);
        BeveledWedge* wedge_frame23_ptr3 = new BeveledWedge(y80_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame23, phi1_frame23);
        wedge_frame23_ptr3->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame23_ptr3);

        BeveledWedge* wedge_door_ptr2 = new BeveledWedge(y30_unit, y51_unit, r20_unit, r21_unit, rb2_unit, phi0_frame23, phi1_frame23);
        wedge_door_ptr2->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_door_ptr2);
        BeveledWedge* wedge_window3_ptr1 = new BeveledWedge(y70_unit, y71_unit, r20_unit, r21_unit, rb2_unit, phi0_frame23, phi1_frame23);
        wedge_window3_ptr1->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_window3_ptr1);


        double phi0_frame14 = phi1_frame23;
        double phi1_frame14 = phi0_frame14 + _frame_angle_width;

        BeveledWedge* wedge_frame14_ptr = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_frame14, phi1_frame14);
        wedge_frame14_ptr->set_material(sv_phong_frame_ptr1);
        door_wall_grid_ptr->add_object(wedge_frame14_ptr);


        double phi0_frame24 = phi1_frame14;
        double phi1_frame24 = phi0_frame24 + _window_angle_width;

        BeveledWedge* wedge_frame24_ptr1 = new BeveledWedge(y20_unit, y21_unit, r10_unit, r11_unit, rb1_unit, phi0_frame24, phi1_frame24);
        wedge_frame24_ptr1->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame24_ptr1);
        BeveledWedge* wedge_frame24_ptr2 = new BeveledWedge(y40_unit, y41_unit, r10_unit, r11_unit, rb1_unit, phi0_frame24, phi1_frame24);
        wedge_frame24_ptr2->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame24_ptr2);
        BeveledWedge* wedge_frame24_ptr3 = new BeveledWedge(y60_unit, y61_unit, r10_unit, r11_unit, rb1_unit, phi0_frame24, phi1_frame24);
        wedge_frame24_ptr3->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame24_ptr3);
        BeveledWedge* wedge_frame24_ptr4 = new BeveledWedge(y80_unit, y81_unit, r10_unit, r11_unit, rb1_unit, phi0_frame24, phi1_frame24);
        wedge_frame24_ptr4->set_material(sv_phong_frame_ptr2);
        door_wall_grid_ptr->add_object(wedge_frame24_ptr4);

        BeveledWedge* wedge_window4_ptr1 = new BeveledWedge(y30_unit, y31_unit, r20_unit, r21_unit, rb2_unit, phi0_frame24, phi1_frame24);
        wedge_window4_ptr1->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_window4_ptr1);
        BeveledWedge* wedge_window4_ptr2 = new BeveledWedge(y50_unit, y51_unit, r20_unit, r21_unit, rb2_unit, phi0_frame24, phi1_frame24);
        wedge_window4_ptr2->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_window4_ptr2);
        BeveledWedge* wedge_window4_ptr3 = new BeveledWedge(y70_unit, y71_unit, r20_unit, r21_unit, rb2_unit, phi0_frame24, phi1_frame24);
        wedge_window4_ptr3->set_material(sv_glossy_window_ptr);
        door_wall_grid_ptr->add_object(wedge_window4_ptr3);


        double phi0_frame15 = phi1_frame24;
        double phi1_frame15 = phi0_frame15 + _frame_angle_width;

        BeveledWedge* wedge_frame15_ptr = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_frame15, phi1_frame15);
        wedge_frame15_ptr->set_material(sv_phong_frame_ptr1);
        door_wall_grid_ptr->add_object(wedge_frame15_ptr);


        double phi0_wall2 = phi1_frame15;
        double phi1_wall2 = phi1_unit;

        BeveledWedge* wedge_wall_ptr2 = new BeveledWedge(y10_unit, y11_unit, r10_unit, r11_unit, rb1_unit, phi0_wall2, phi1_wall2);
        wedge_wall_ptr2->set_material(sv_phong_wall_ptr1);
        door_wall_grid_ptr->add_object(wedge_wall_ptr2);
    }
    door_wall_grid_ptr->setup_cells();
    scene_grid_ptr->add_object(door_wall_grid_ptr);


    ////////////////////////////////////////dragon////////////////////////////////////////////////

    char* file_dragon_name = "/Users/libingzeng/CG/RayTraceGroundUp/PLYFiles/dragon.ply";
    Grid* grid_dragon_ptr = new Grid(new Mesh);
    grid_dragon_ptr->read_smooth_triangles(file_dragon_name);
    grid_dragon_ptr->setup_cells();

    // noise:

    CubicNoise* noise_dragon_ptr = new CubicNoise;
    noise_dragon_ptr->set_num_octaves(6);
    noise_dragon_ptr->set_gain(0.5);
    noise_dragon_ptr->set_lacunarity(2.0);

    // ramp image:

    Image* image_dragon_ptr = new Image;
    image_dragon_ptr->read_ppm_file("/Users/libingzeng/CG/RayTraceGroundUp/TextureFiles/ppm/BlueMarbleRamp.ppm");

    // marble texture:

    FBmTextureRamp* marble_dragon_ptr = new FBmTextureRamp(image_dragon_ptr);
    marble_dragon_ptr->set_noise(noise_dragon_ptr);
    marble_dragon_ptr->set_perturbation(4.0);

    InstanceTexture* it_dragon_ptr = new InstanceTexture(marble_dragon_ptr);
    it_dragon_ptr->scale(0.05);
    //	it_dragon_ptr->rotate_z(110);
    //	it_dragon_ptr->translate(1.0, 4.0, 0.0);


        // material:

    SV_Phong* sv_phong_dragon_ptr = new SV_Phong;
    sv_phong_dragon_ptr->set_ka(0.25);
    sv_phong_dragon_ptr->set_kd(0.75);
    sv_phong_dragon_ptr->set_cd(it_dragon_ptr);
    sv_phong_dragon_ptr->set_ks(0.1);
    sv_phong_dragon_ptr->set_exp(20.0);


    Instance* instance_dragon_ptr = new Instance(grid_dragon_ptr);
    instance_dragon_ptr->scale(20, 10, 20);
    instance_dragon_ptr->translate(0, -0.3, 0);
    instance_dragon_ptr->set_material(sv_phong_dragon_ptr);

    scene_grid_ptr->add_object(instance_dragon_ptr);

    scene_grid_ptr->setup_cells();
    add_object(scene_grid_ptr);
}
#endif // TYPE
