module;

#include "pch.h"

export module mod_dem;

import mod_render;

using namespace DirectX;

export namespace DE::Render::DEM {

    class Vertex {
    public:
        uint64_t id;
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT2 tex_coord;
        XMFLOAT4 color;
    };

    class Mesh {
    public:
        // Mesh Storage
        std::vector<Vertex> 
            verts;

        std::vector<Material> 
            materials;

        // Cache Storage 
        uint64_t 
            vertexBufferObject;

        // Mesh Local Transformation State
        XMFLOAT3
            position,
            rotation,
            scale;

        XMFLOAT4X4
            transform;

        Mesh() {}
        ~Mesh() {}

        /// <summary>
        /// Uses the data found within the mesh storage properties to send the data to the graphics device
        /// </summary>
        void Cache() {
            D3D11_BUFFER_DESC bufferDescription;
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_DEFAULT;
            bufferDescription.ByteWidth = verts.size() * sizeof(verts[0]);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bufferDescription.CPUAccessFlags = 0;

            std::cout << "created buffer description" << std::endl;

        }

        void Draw() {}
    };

    class Model {
    public:
        std::vector<Mesh> 
            meshes;

        // Model Transformation 
        XMFLOAT3 
            position, 
            rotation, 
            scale;

        XMFLOAT4X4
            transform;


        Model() {}
        ~Model() {}

        void DrawImmediate() {}
        void DrawInBatch() {}

        static void LoadFromFile(std::filesystem::path filepath) {

        }

//#ifdef _DEBUG
        static void CreateTestModel(std::unique_ptr<Model> &out) {

            // create the model object
            out = std::make_unique<Model>();
            
            // create submesh 
            auto _mesh = Mesh();
            // load vert data
            _mesh.verts = {
                { 1, XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                { 2, XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
                { 3, XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
            };

            std::cout << "Sending test mesh to the cache" << std::endl;

            _mesh.Cache();

        }
//#endif
    };

};