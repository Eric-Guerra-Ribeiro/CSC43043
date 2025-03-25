#include "tree.hpp"

using namespace cgp;


mesh create_cylinder_mesh(float radius, float height)
{
    mesh m; 
    // To do: fill this mesh ...
    // ...
    // To add a position: 
    //   m.position.push_back(vec3{x,y,z})
    // Or in pre-allocating the buffer:
    //   m.position.resize(maximalSize);
    //   m.position[index] = vec3{x,y,z}; (with 0<= index < maximalSize)

    // u: angle, v: height

    int N_u = 20;
    int N_v = 2;

    m.position.resize((N_u - 1)*N_v);

    for (int ku = 0; ku < N_u - 1; ++ku)
    {
        for (int kv = 0; kv < N_v; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku / (N_u - 1.0f);
            float v = kv / (N_v - 1.0f);

            // Compute the real coordinates (x,y, z)
            float x = radius*cos(2*Pi*u);
            float y = radius*sin(2*Pi*u);
            float z = (kv == 0 ? 0.0f : height);

            // Store vertex coordinates
            m.position[kv + N_v * ku] = {x,y,z};
        }
    }

    unsigned int num_vertices = N_v * (N_u - 1);
    // Similar with the triangle connectivity:
    //  m.connectivity.push_back(uint3{index_1, index_2, index_3});
    for (int ku = 0; ku < N_u - 1; ++ku)
    {
        for (int kv = 0; kv < N_v - 1; ++kv)
        {
            unsigned int idx = kv + N_v * ku; // current vertex offset

            uint3 triangle_1 = { (idx + 2) % num_vertices, (idx + 1) % num_vertices, (idx) % num_vertices };
            uint3 triangle_2 = { (idx + 2) % num_vertices, (idx + 3) % num_vertices, (idx + 1)%num_vertices };

            m.connectivity.push_back(triangle_1);
            m.connectivity.push_back(triangle_2);
        }
    }

    // Need to call fill_empty_field() before returning the mesh 
    //  this function fill all empty buffer with default values (ex. normals, colors, etc).
    m.fill_empty_field();

    return m;
}

mesh create_cone_mesh(float radius, float height, float z_offset)
{
    mesh m; 

    // u: angle

    int N_u = 20;

    // + 2 because we add one vertex for the center of the base circle and another for the apex of the cone
    m.position.resize((N_u - 1) + 2);
    // Cone apex and center of base circule
    int center_idx = m.position.size() - 2;
    int apex_idx = m.position.size() - 1;
    m.position[center_idx] = { 0, 0, z_offset };
    m.position[apex_idx] = {0, 0, height + z_offset};

    for (int ku = 0; ku < N_u - 1; ++ku)
    {
        // Compute local parametric coordinates (u) \in [0,1]
        float u = ku / (N_u - 1.0f);

        // Compute the real coordinates (x,y, z)
        float x = radius * cos(2 * Pi * u);
        float y = radius * sin(2 * Pi * u);
        float z = z_offset;

        // Store vertex coordinates
        m.position[ku] = { x,y,z };
    }

    unsigned int num_vertices_base = (N_u - 1);
    // Similar with the triangle connectivity:
    //  m.connectivity.push_back(uint3{index_1, index_2, index_3});
    for (int ku = 0; ku < N_u; ++ku)
    {
        uint3 triangle_1 = {center_idx, (ku) % num_vertices_base, (ku + 1) % num_vertices_base };
        uint3 triangle_2 = { (ku + 1) % num_vertices_base, apex_idx, (ku) % num_vertices_base };

        m.connectivity.push_back(triangle_1);
        m.connectivity.push_back(triangle_2);
    }

    // Need to call fill_empty_field() before returning the mesh 
    //  this function fill all empty buffer with default values (ex. normals, colors, etc).
    m.fill_empty_field();

    return m;
}


mesh create_tree()
{
    float h = 0.7f; // trunk height
    float r = 0.1f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_cylinder_mesh(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });


    // Create a green foliage from 3 cones
    mesh foliage = create_cone_mesh(4 * r, 6 * r, 0.0f);      // base-cone
    foliage.push_back(create_cone_mesh(4 * r, 6 * r, 2 * r));   // middle-cone
    foliage.push_back(create_cone_mesh(4 * r, 6 * r, 4 * r));   // top-cone
    foliage.translate({ 0,0,h });       // place foliage at the top of the trunk
    foliage.color.fill({ 0.4f, 0.6f, 0.3f });

    // The tree is composed of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}


// Mushrooms aren't trees, but this file is convenient
mesh create_mushroom() {
    float h = 0.1f; // stem height
    float r = 0.02f; // stem radius

    // Create a whitish trunk
    mesh trunk = create_cylinder_mesh(r, h);
    trunk.color.fill({ 0.9f, 0.9f, 0.9f });


    // Create the cap from a cone
    mesh cap = create_cone_mesh(4 * r, 2 * r, h);
    cap.color.fill({ 0.9f, 0.15f, 0.13f });

    // The tree is composed of the trunk and the foliage
    mesh mushroom = trunk;
    mushroom.push_back(cap);

    return mushroom;
}
