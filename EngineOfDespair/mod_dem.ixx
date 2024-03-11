module;

#include "pch.h"

export module mod_dem;

import mod_render;
import mod_strtools;

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
        std::string
            name = "NONAME";

        // Mesh Storage
        std::vector<Vertex> 
            verts = {};

        std::vector<Material> 
            materials = {};

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
            bufferDescription.ByteWidth = (unsigned int)verts.size() * sizeof(verts[0]);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bufferDescription.CPUAccessFlags = 0;

            std::cout << "created buffer description" << std::endl;

        }

        void Draw() {}
    };

    class Model {
    public:
        uint32_t
            version = 0,
            num_meshes = 0,
            num_joints = 0,
            num_weights = 0;

        std::string 
            name = "NO_NAME";

        std::vector<std::unique_ptr<Mesh>> 
            meshes = {};

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

        void addMesh(std::unique_ptr<Mesh> &_newMesh) {
            if (_newMesh->verts.size() <= 0)
                return;

            meshes.push_back(std::move(_newMesh));
        }

        /// <summary>
        /// Reads a DEM file 
        /// </summary>
        /// <param name="filepath"></param>
        //static void LoadFromFile(std::filesystem::path filepath, std::unique_ptr<Model> &out) {
        //    out = std::make_unique<Model>();


        //}

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


    void LoadFromFile(std::filesystem::path filepath, std::unique_ptr<Model> &out) {
        
        enum class MODE { HEADER = 1, MODEL, ANIM };
        MODE _mode = MODE::HEADER;
        enum class SUBMODE { NONE = 1, MESH, SEQUENCE };
        SUBMODE _submode = SUBMODE::NONE;

        enum class TOKEN { COMMENT, VERSION, 
            MODEL, 
                JOINTS_NUM, MESHES_NUM, SEQUENCES_NUM, WEIGHTS_NUM, MESH, MATERIAL, VERTS_NUM, VERT, TRIS_NUM, TRI, WEIGHT, HIERARCHY_JOINT,
            SEQUENCE,
                RATE, COMPONENTS_NUM, SEQ_JOINTS_NUM, SEQ_JOINT, SEQ_BOUNDS_NUM, SEQ_BOUND, SEQ_EVENTS_NUM, SEQ_EVENT, SEQ_FRAMES_NUM, SEQ_FRAME
                
        };
        TOKEN _current_token = TOKEN::COMMENT;

        std::map<std::string, TOKEN> _machineTokenMap = {
            { "#", TOKEN::COMMENT },
            { "//", TOKEN::COMMENT },
            { "DEM", TOKEN::VERSION },
        };
        std::map<std::string, TOKEN> _modelTokenMap = {
            // Model Tokens
            { "model", TOKEN::MODEL },
            { "joints", TOKEN::JOINTS_NUM },
            { "meshes", TOKEN::MESHES_NUM },
            { "sequences", TOKEN::SEQUENCES_NUM },
            { "mesh", TOKEN::MESH },
            { "mat", TOKEN::MATERIAL },
            { "verts", TOKEN::VERTS_NUM },
            { "v", TOKEN::VERT },
            { "tris", TOKEN::TRIS_NUM },
            { "t", TOKEN::TRI },
            { "weights", TOKEN::WEIGHTS_NUM },
            { "w", TOKEN::WEIGHT },
            { "h", TOKEN::HIERARCHY_JOINT },
        };
        std::map<std::string, TOKEN> _animTokenMap = {
            { "sequence", TOKEN::SEQUENCE },
            { "rate", TOKEN::RATE },
            { "components", TOKEN::COMPONENTS_NUM },
            { "joints", TOKEN::SEQ_JOINTS_NUM },
            { "j", TOKEN::SEQ_JOINT },
            { "bounds", TOKEN::SEQ_BOUNDS_NUM },
            { "b", TOKEN::SEQ_BOUND },
            { "events", TOKEN::SEQ_EVENTS_NUM },
            { "e", TOKEN::SEQ_EVENT },
            { "frames", TOKEN::SEQ_FRAMES_NUM },
            { "f", TOKEN::SEQ_FRAME }
        };
        std::map<MODE, std::map<std::string, TOKEN>> _tokenMap = {
            { MODE::HEADER, _machineTokenMap },
            { MODE::MODEL, _modelTokenMap },
            { MODE::ANIM, _animTokenMap }
        };

        
        out = std::make_unique<Model>();

        //auto active_mesh = std::make_unique<Mesh>();
        // what might be better is to let the model instance create the mesah instance and we just get the ref to work with
        // this way we wont need to deal with cleanup and the mesh is just assumed complete
        //auto active_mesh = out->createMesh();

        std::ifstream file(filepath);

        // Phase 1: Read the data from file
        int line_num = 0;
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line)) {
                auto parts = StringTools::str_split(StringTools::str_trim(line), ' ');

                std::cout << line << std::endl;

                // Read token, determine mode
                if (_machineTokenMap.find(parts[0]) != _machineTokenMap.end()) {
                    switch (_machineTokenMap[parts[0]])
                    {
                    case TOKEN::COMMENT:
                        // if a comment line, move on 
                        continue;
                        break;
                    case TOKEN::VERSION:
                        out->version = std::stoi(parts[1]);
                        break;
                    default:
                        break;
                    }
                    continue; // skip the rest of the loop
                }

                if (_modelTokenMap.find(parts[0]) != _modelTokenMap.end()) {
                    switch (_modelTokenMap[parts[0]])
                    {
                    case TOKEN::MODEL:
                        _mode = MODE::MODEL;
                        out->name = parts[1];
                        break;
                    case TOKEN::JOINTS_NUM:
                        out->num_joints = std::stoi(parts[1]);
                        break;
                    case TOKEN::MESHES_NUM:
                        out->num_meshes = std::stoi(parts[1]);
                        break;
                    case TOKEN::WEIGHTS_NUM:
                        out->num_weights = std::stoi(parts[1]);
                        break;


                    case TOKEN::MESH:
                        _submode = SUBMODE::MESH;
                        // add the activly modified mesh instance to the model. However, there must be at least a vert to be considered a "mesh"
                        out->addMesh(active_mesh);
                        active_mesh = std::make_unique<Mesh>();
                        active_mesh->name = parts[1];
                        break;
                    case TOKEN::MATERIAL:
                        break;
                    case TOKEN::VERTS_NUM:
                        break;
                    case TOKEN::VERT:
                        break;
                    default:
                        break;
                    }

                    continue; // skip the rest of the loop
                }
                
                // do something with that mode

                line_num++;
            }
        }

        // if we've reached end of file but we were still building a mesh, add it
        out->addMesh(active_mesh);

        file.close();
    }
};