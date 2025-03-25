
#include "terrain.hpp"


using namespace cgp;

// Evaluate 3D position of the terrain for any (x,y)
float evaluate_terrain_height(float x, float y)
{

    int const N_GAUSSIANS = 4;

    std::array<vec2, N_GAUSSIANS> p_i = {vec2{-10,-10}, vec2{5,5}, vec2{-3,4}, vec2{6,4}};
    std::array<float, N_GAUSSIANS> h_i = {3.0f, -1.5f, 1.0f, 2.0f};
    std::array<float, N_GAUSSIANS> sigma_i = {10.0f, 3.0f, 4.0f, 4.0f};

    float d;
    float z = 0;

    for (int i = 0; i < N_GAUSSIANS; ++i) {
        d = norm(vec2(x, y) - p_i[i]) / sigma_i[i];

        z += h_i[i] * std::exp(-d * d);
    }
    return z;
}

mesh create_terrain_mesh(int N, float terrain_length)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);

    // Fill terrain geometry
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku/(N-1.0f);
            float v = kv/(N-1.0f);

            // Compute the real coordinates (x,y) of the terrain in [-terrain_length/2, +terrain_length/2]
            float x = (u - 0.5f) * terrain_length;
            float y = (v - 0.5f) * terrain_length;

            // Compute the surface height function at the given sampled coordinate
            float z = evaluate_terrain_height(x,y);

            // Store vertex coordinates
            terrain.position[kv+N*ku] = {x,y,z};
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(int ku=0; ku<N-1; ++ku)
    {
        for(int kv=0; kv<N-1; ++kv)
        {
            unsigned int idx = kv + N*ku; // current vertex offset

            uint3 triangle_1 = {idx, idx+1+N, idx+1};
            uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
	terrain.fill_empty_field(); 

    return terrain;
}


bool validate_position(float x, float y, const std::vector<vec3> positions, float min_dist) {
    if (min_dist < 1.0e-12) {
        return true;
    }

    for (auto& position : positions) {
        if (norm(position - vec3(x, y, position.z)) < min_dist) {
            return false;
        }
    }
    return true;
}


std::vector<vec3> generate_positions_on_terrain(int N, float terrain_length, float min_dist) {
    std::vector<vec3> positions;

    positions.reserve(N);

    float x;
    float y;

    int tries = 0;
    int const MAX_TRIES = 1000;

    for (int i = 0; i < N; ++i) {
        tries = 0;
        do {
            ++tries;
            x = rand_uniform(-0.5f * terrain_length, 0.5f * terrain_length);
            y = rand_uniform(-0.5f * terrain_length, 0.5f * terrain_length);
            if (tries > MAX_TRIES) {
                throw std::runtime_error("Too hard/impossible to find compatible positions");
            }
        } while (!validate_position(x, y, positions, min_dist));
        positions.emplace_back(x, y, evaluate_terrain_height(x, y));
    }

    return positions;
}
