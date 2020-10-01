//#####################################################################
// Main
// Dartmouth COSC 77/177 Computer Graphics, technical project starter code
//#####################################################################
#include <iostream>

#include "Common.h"
#include "Driver.h"
#include "Particles.h"
#include "OpenGLMesh.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLParticles.h"
#include "TinyObjLoader.h"
#include "PerlinNoise.h"
//#include "Framework.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GLUT/glut.h>
#include "texload.h"

float skyboxVertices[] = {
	// positions
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};


class TechProjDriver : public Driver, public OpenGLViewer
{
	using Base = Driver;
public:
	Array<TriangleMesh<3>* > triangle_meshes;
	OpenGLTriangleMesh* opengl_Ptri_mesh;
  OpenGLTriangleMesh* opengl_BPtri_mesh;
  OpenGLTriangleMesh* opengl_SStri_mesh;
  OpenGLTriangleMesh* opengl_SBtri_mesh;
  OpenGLTriangleMesh* opengl_Ktri_mesh;
	//int noise[500][500];



	virtual void Initialize()
	{
		OpenGLViewer::Initialize();

	}

	virtual void Run()
	{
		OpenGLViewer::Run();
	}

	////Initialize data in the scene
	virtual void Initialize_Data()
	{

		/*cv::Mat img = cv::imread("models/noiseTexture.png");
   	for(int i=0;i<img.cols;i++)
      {
         for(int j=0;j<img.rows;j++)
            {
               noise[i][j]=img.at<int>(i,j);
            }
      }*/

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Init_Shaders();
		Init_Textures();
		//Init_Background();

		Init_Bunny_Mesh();
		Init_Plane_Mesh();

		//Init_Sphere_Mesh();

		Init_Skybox();

        Init_ShipBody_Mesh();
        Init_ShipSail_Mesh();
        for (int i = 0; i < 100; i++) {
            Init_Kelp_Mesh((rand()%50)+50, (rand()%360)*M_PI/180.0,
                           ((rand()%2)*2-1)*((rand()%100)+10.0), -10.0, ((rand()%2)*2-1)*((rand()%100)+10.0));
        }
        for (int i = 0; i < 3; i++) {
            Init_Rock1_Mesh((rand()%25)+75, (rand()%360)*M_PI/180.0,
                           ((rand()%2)*2-1)*((rand()%100)), -10.0, ((rand()%2)*2-1)*((rand()%100)));
        }
        for (int i = 0; i < 2; i++) {
            Init_Rock2_Mesh((rand()%25)+75, (rand()%360)*M_PI/180.0,
                            ((rand()%2)*2-1)*((rand()%100)), -10.0, ((rand()%2)*2-1)*((rand()%100)));
        }
        for (int i = 0; i < 10; i++) {
            Init_Jellyfish_Mesh((rand()%3)+1.0, (rand()%360)*M_PI/180.0,
                            ((rand()%2)*2-1)*((rand()%50)), 5.0, ((rand()%2)*2-1)*((rand()%50)));
        }
        Init_Lighting();
	}

	//////////////////////////////////////////////////////////////////////////
	////Specifying your own lighting effects in this function
	////In this example we initialized two directional lights, the same as Assignment 2
	void Init_Lighting() {
		auto dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(-1.f, -1.f, -1.f));
		dir_light->dif = glm::vec4(.9,.8,.7, 1.0);

		dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(1.f, -1.f, -1.f));
		dir_light->dif = glm::vec4(.7,.7,.7, 1.0);

		OpenGLUbos::Set_Ambient(glm::vec4(.01f, .01f, .02f, 1.f));
		OpenGLUbos::Update_Lights_Ubo();
	}

	//////////////////////////////////////////////////////////////////////////
	////Write your own vertex shader and fragment shader and add it to the shader library
	void Init_Shaders()
	{
		Add_Shader("shaders/background.vert","shaders/background.frag","background");
		Add_Shader("shaders/shared.vert", "shaders/lamb_textured.frag", "lamb_tex");
		Add_Shader("shaders/shared.vert", "shaders/lamb.frag", "lamb");
		Add_Shader("shaders/skybox.vert", "shaders/skybox.frag", "skybox");
		Add_Shader("shaders/shared.vert", "shaders/sand.frag", "sand");
        Add_Shader("shaders/shared.vert", "shaders/ship.frag", "ship");
        Add_Shader("shaders/shared.vert", "shaders/sails.frag", "sails");
        Add_Shader("shaders/shared.vert", "shaders/phong_tex.frag", "phong");
        Add_Shader("shaders/shared.vert", "shaders/jellyfish.frag", "jelly");
	}

	//////////////////////////////////////////////////////////////////////////
	////Read a texture from a image and add it to the texture library
	void Init_Textures()
	{
		Add_Texture("models/bunny.jpg","bunny");

		Add_Texture("models/skybox/face4.jpg", "sand");
		Add_Texture("models/caust/caust00.jpg", "caust1");
        Add_Texture("models/caust/caust02.jpg", "caust2");
        Add_Texture("models/caust/caust04.jpg", "caust3");
        Add_Texture("models/caust/caust06.jpg", "caust4");
        Add_Texture("models/caust/caust08.jpg", "caust5");
        Add_Texture("models/caust/caust10.jpg", "caust6");
        Add_Texture("models/caust/caust12.jpg", "caust7");
        Add_Texture("models/caust/caust14.jpg", "caust8");
        Add_Texture("models/caust/caust16.jpg", "caust9");
        Add_Texture("models/caust/caust18.jpg", "caust10");
        Add_Texture("models/caust/caust20.jpg", "caust11");
        Add_Texture("models/caust/caust22.jpg", "caust12");
        Add_Texture("models/caust/caust24.jpg", "caust13");
        Add_Texture("models/caust/caust26.jpg", "caust14");
        Add_Texture("models/caust/caust28.jpg", "caust15");
        Add_Texture("models/caust/caust30.jpg", "caust16");

        Add_Texture("models/ship/Ship_diff.jpg","ship_tex");
        Add_Texture("models/ship/Ship_glow.jpg","ship_glow");
        Add_Texture("models/ship/Ship_norm.jpg","ship_norm");
        Add_Texture("models/ship/Ship_spec.jpg","ship_spec");
        Add_Texture("models/ship/Sails_diff.jpg","sails_tex");
        Add_Texture("models/ship/Sails_alpha.jpg","sails_alpha");

        Add_Texture("models/kelp/kelp_albedo.jpg","kelp_tex");
        Add_Texture("models/kelp/kelp_normal.png","kelp_norm");
        Add_Texture("models/kelp/kelp_metallic.jpeg","kelp_metal");
        Add_Texture("models/kelp/kelp_roughness.jpeg","kelp_rough");
        Add_Texture("models/kelp/kelp_opacity.jpeg","kelp_alpha");

        Add_Texture("models/rock1/Rock1_d.jpg","rock1_tex");
        Add_Texture("models/rock1/Rock1_n.jpg","rock1_norm");
        Add_Texture("models/rock1/Rock1_s.jpg","rock1_metal");

        Add_Texture("models/rock2/rock2_albedo.png","rock2_tex");
        Add_Texture("models/rock2/rock2_normal.png","rock2_norm");
        //Add_Texture("models/rock2/rock2_rough.png","rock2_rough"); // For some reason causes a segfault

        Add_Texture("models/cliff/cliff_albedo.jpg","cliff_tex");

        Add_Texture("models/jellyfish/jellyfish_a.jpeg","jelly_tex");
        Add_Texture("models/jellyfish/jellyfish_e.jpeg","jelly_emiss");
        Add_Texture("models/jellyfish/jellyfish_n.jpeg","jelly_n");

		std::vector<std::string> faces =    {"models/skybox/face1.jpg", "models/skybox/face2.jpg", "models/skybox/face3.jpg",
                                            "models/skybox/face4.jpg", "models/skybox/face5.jpg", "models/skybox/face6.jpg"};
		Add_Cube_Texture(faces, "skybox");

	}

	//////////////////////////////////////////////////////////////////////////
	////Initialize the background shader
	void Init_Background()
	{
		OpenGLBackground* opengl_background=Add_Interactive_Object<OpenGLBackground>();
		opengl_background->shader_name="background";
		opengl_background->Initialize();
	}

	//////////////////////////////////////////////////////////////////////////
	////Init a mesh and bind shader and texture
	////To make your own mesh, copy this function and modify the variables
	void Init_Bunny_Mesh()
	{
		////Initialize the mesh file, shader, and texture of the mesh
		std::string mesh_file_name = "models/bunny.obj";
		std::string shader_name = "sand";
		std::string texture_name = "bunny";

		////Read mesh from obj file
		opengl_BPtri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
    std::vector<int> dummy;
    dummy.push_back(0);
		Read_Mesh(mesh_file_name, opengl_BPtri_mesh->mesh, dummy);
		Rescale(opengl_BPtri_mesh->mesh.Vertices(),1.);
		Translate_Center_To(opengl_BPtri_mesh->mesh.Vertices(),Vector3::Ones()*.5);

		////Initialize the model matrix
		opengl_BPtri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		////Other mesh initialization setups
		Set_Mesh_Default_Options(opengl_BPtri_mesh);

		////Bind an initialized shader to the mesh
		opengl_BPtri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("sand"));

		////Bind a texture to the mesh
    opengl_BPtri_mesh->Clear_Textures();
		opengl_BPtri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture("sand"));
		opengl_BPtri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust1"));
    opengl_BPtri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust2"));
    opengl_BPtri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust3"));
    opengl_BPtri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust4"));
    opengl_BPtri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust5"));

		////Add the triangle mesh to the array to access the mesh later
		TriangleMesh<3>* triangle_mesh=&opengl_BPtri_mesh->mesh;
		triangle_meshes.push_back(triangle_mesh);
	}


    /*
     * Initializes a Jellyfish
     */
    void Init_Jellyfish_Mesh(float s, float r, float dx, float dy, float dz)
    {
        ////Initialize the mesh file, shader, and texture of the mesh
        std::string mesh_file_name = "models/jellyfish/jellyfish.obj";
        std::string shader_name = "jelly";
        std::string texture_name = "jelly_tex";


        ////Read mesh from obj file
        OpenGLTriangleMesh* opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
        // There are 94 friggin shapes in this .obj file
        std::vector<int> desired_shapes; desired_shapes.push_back(0);
        Read_Mesh(mesh_file_name, opengl_tri_mesh->mesh, desired_shapes);
        Rescale(opengl_tri_mesh->mesh.Vertices(), s);
        //Translate_Center_To(opengl_tri_mesh->mesh.Vertices(),Vector3::Ones()*.5);

        ////Initialize the model matrix

        glm::mat4 m = glm::scale(glm::mat4(1.0f), glm::vec3(1.00, 2.5, 1.00));
        m = glm::rotate(m, r , glm::vec3(0,1,0));
        opengl_tri_mesh->model_matrix = glm::translate(m, glm::vec3(dx, dy, dz));

        ////Other mesh initialization setups
        Set_Mesh_Default_Options(opengl_tri_mesh);

        ////Bind an initialized shader to the mesh
        opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

        ////Bind a texture to the mesh
        opengl_tri_mesh->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture(texture_name));


        std::cout<<opengl_tri_mesh->mesh.Elements().size()<<std::endl;
        ////Add the triangle mesh to the array to access the mesh later
        TriangleMesh<3>* triangle_mesh=&opengl_tri_mesh->mesh;
        triangle_meshes.push_back(triangle_mesh);
    }


    /*
     * Initializes a cliff
     */
    void Init_Cliff_Mesh()
    {
        ////Initialize the mesh file, shader, and texture of the mesh
        std::string mesh_file_name = "models/cliff/cliff.obj";
        std::string shader_name = "phong";
        std::string texture_name = "cliff_tex";
        //std::string normal_name = "rock2_norm";


        ////Read mesh from obj file
        OpenGLTriangleMesh* opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
        // There are 94 friggin shapes in this .obj file
        std::vector<int> desired_shapes; desired_shapes.push_back(0);
        Read_Mesh(mesh_file_name, opengl_tri_mesh->mesh, desired_shapes);
        Rescale(opengl_tri_mesh->mesh.Vertices(),30.);
        //Translate_Center_To(opengl_tri_mesh->mesh.Vertices(),Vector3::Ones()*.5);

        ////Initialize the model matrix
        opengl_tri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(20, 20, 0));

        ////Other mesh initialization setups
        Set_Mesh_Default_Options(opengl_tri_mesh);

        ////Bind an initialized shader to the mesh
        opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

        ////Bind a texture to the mesh
        opengl_tri_mesh->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture(texture_name));
        //opengl_tri_mesh->Add_Texture("normal", OpenGLTextureLibrary::Get_Texture(normal_name));

        std::cout<<opengl_tri_mesh->mesh.Elements().size()<<std::endl;
        ////Add the triangle mesh to the array to access the mesh later
        TriangleMesh<3>* triangle_mesh=&opengl_tri_mesh->mesh;
        triangle_meshes.push_back(triangle_mesh);
    }

    /*
     * Initializes Rock 2. Can be any of the 3 rocks "Rock2_3", "Rock2_2", "Rock2_1"
     */
    void Init_Rock2_Mesh(float s, float r, float dx, float dy, float dz)
    {
        ////Initialize the mesh file, shader, and texture of the mesh
        std::string mesh_file_name = "models/rock2/Rock2_3.obj";
        std::string shader_name = "phong";
        std::string texture_name = "rock2_tex";
        std::string normal_name = "rock2_norm";
        //std::string rough_name = "rock2_rough";


        ////Read mesh from obj file
        OpenGLTriangleMesh* opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
        // There are 94 friggin shapes in this .obj file
        std::vector<int> desired_shapes; desired_shapes.push_back(0);
        Read_Mesh(mesh_file_name, opengl_tri_mesh->mesh, desired_shapes);
        Rescale(opengl_tri_mesh->mesh.Vertices(), s);
        //Translate_Center_To(opengl_tri_mesh->mesh.Vertices(),Vector3::Ones()*.5);

        ////Initialize the model matrix
        glm::mat4 m = glm::rotate(glm::mat4(1.0f), r , glm::vec3(0,1,0));
        opengl_tri_mesh->model_matrix = glm::translate(m, glm::vec3(dx, dy, dz));

        ////Other mesh initialization setups
        Set_Mesh_Default_Options(opengl_tri_mesh);

        ////Bind an initialized shader to the mesh
        opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

        ////Bind a texture to the mesh
        opengl_tri_mesh->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture(texture_name));
        opengl_tri_mesh->Add_Texture("normal", OpenGLTextureLibrary::Get_Texture(normal_name));
        //opengl_tri_mesh->Add_Texture("roughness", OpenGLTextureLibrary::Get_Texture(rough_name));

        std::cout<<opengl_tri_mesh->mesh.Elements().size()<<std::endl;
        ////Add the triangle mesh to the array to access the mesh later
        TriangleMesh<3>* triangle_mesh=&opengl_tri_mesh->mesh;
        triangle_meshes.push_back(triangle_mesh);
    }

    /*
     * Initializes Rock 1
     */
    void Init_Rock1_Mesh(float s, float r, float dx, float dy, float dz)
    {
        ////Initialize the mesh file, shader, and texture of the mesh
        std::string mesh_file_name = "models/rock1/Rock1.obj";
        std::string shader_name = "phong";
        std::string texture_name = "rock1_tex";
        std::string normal_name = "rock1_norm";
        std::string metal_name = "rock1_metal";


        ////Read mesh from obj file
        OpenGLTriangleMesh* opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
        // There are 94 friggin shapes in this .obj file
        std::vector<int> desired_shapes; desired_shapes.push_back(0);
        Read_Mesh(mesh_file_name, opengl_tri_mesh->mesh, desired_shapes);
        Rescale(opengl_tri_mesh->mesh.Vertices(), s);
        //Translate_Center_To(opengl_tri_mesh->mesh.Vertices(),Vector3::Ones()*.5);

        ////Initialize the model matrix
        glm::mat4 m = glm::rotate(glm::mat4(1.0f), r , glm::vec3(0,1,0));
        opengl_tri_mesh->model_matrix = glm::translate(m, glm::vec3(dx, dy, dz));

        ////Other mesh initialization setups
        Set_Mesh_Default_Options(opengl_tri_mesh);

        ////Bind an initialized shader to the mesh
        opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

        ////Bind a texture to the mesh
        opengl_tri_mesh->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture(texture_name));
        opengl_tri_mesh->Add_Texture("normal", OpenGLTextureLibrary::Get_Texture(normal_name));
        opengl_tri_mesh->Add_Texture("metallic", OpenGLTextureLibrary::Get_Texture(metal_name));

        std::cout<<opengl_tri_mesh->mesh.Elements().size()<<std::endl;
        ////Add the triangle mesh to the array to access the mesh later
        TriangleMesh<3>* triangle_mesh=&opengl_tri_mesh->mesh;
        triangle_meshes.push_back(triangle_mesh);
    }

    /*
     * Initializes a kelp
     * Reads textures, etc.
     */
    void Init_Kelp_Mesh(float s, float r, float dx, float dy, float dz)
    {
        ////Initialize the mesh file, shader, and texture of the mesh
        std::string mesh_file_name = "models/kelp/kelp.obj";
        std::string shader_name = "phong";
        std::string texture_name = "kelp_tex";
        std::string normal_name = "kelp_norm";
        std::string metal_name = "kelp_metal";
        std::string rough_name = "kelp_rough";
        std::string alpha_name = "kelp_alpha";
        std::string emiss_name = "kelp_rough";


        ////Read mesh from obj file
        opengl_Ktri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
        // There are 94 friggin shapes in this .obj file
        std::vector<int> desired_shapes;
        for (int i = 0; i < 94; i++) {
            desired_shapes.push_back(i);
        }
        Read_Mesh(mesh_file_name, opengl_Ktri_mesh->mesh, desired_shapes);
        Rescale(opengl_Ktri_mesh->mesh.Vertices(), s);
        //Translate_Center_To(opengl_Ktri_mesh->mesh.Vertices(),Vector3::Ones()*.5);

        ////Initialize the model matrix
        glm::mat4 m = glm::rotate(glm::mat4(1.0f), r, glm::vec3(0,1,0));
        opengl_Ktri_mesh->model_matrix = glm::translate(m, glm::vec3(dx, dy, dz));

        ////Other mesh initialization setups
        Set_Mesh_Default_Options(opengl_Ktri_mesh);

        ////Bind an initialized shader to the mesh
        opengl_Ktri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

        ////Bind a texture to the mesh
        opengl_Ktri_mesh->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture(texture_name));
        opengl_Ktri_mesh->Add_Texture("normal", OpenGLTextureLibrary::Get_Texture(normal_name));
        opengl_Ktri_mesh->Add_Texture("metallic", OpenGLTextureLibrary::Get_Texture(metal_name));
        opengl_Ktri_mesh->Add_Texture("roughness", OpenGLTextureLibrary::Get_Texture(rough_name));
        opengl_Ktri_mesh->Add_Texture("alpha", OpenGLTextureLibrary::Get_Texture(alpha_name));
        opengl_Ktri_mesh->Add_Texture("emissive", OpenGLTextureLibrary::Get_Texture(emiss_name));

        opengl_Ktri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust1"));
        opengl_Ktri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust2"));
        opengl_Ktri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust3"));
        opengl_Ktri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust4"));
        opengl_Ktri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust5"));

        std::cout<<opengl_Ktri_mesh->mesh.Elements().size()<<std::endl;
        ////Add the triangle mesh to the array to access the mesh later
        TriangleMesh<3>* triangle_mesh=&opengl_Ktri_mesh->mesh;
        triangle_meshes.push_back(triangle_mesh);
    }



  /*
     * Initializes the ship sails
     * Reads textures, etc.
     */
    void Init_ShipSail_Mesh()
    {
        ////Initialize the mesh file, shader, and texture of the mesh
        std::string mesh_file_name = "models/ship/Ship.obj";
        std::string shader_name = "sails";
        std::string texture_name = "sails_tex";
        std::string alpha_name = "sails_alpha";

        ////Read mesh from obj file
        opengl_SStri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
        std::vector<int> desired_shapes; /*desired_shapes.push_back(0);*/ desired_shapes.push_back(2);
        Read_Mesh(mesh_file_name, opengl_SStri_mesh->mesh, desired_shapes);
        Rescale(opengl_SStri_mesh->mesh.Vertices(),10.);
        //Translate_Center_To(opengl_tri_mesh->mesh.Vertices(),Vector3::Ones()*.5);

        ////Initialize the model matrix
        opengl_SStri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        ////Other mesh initialization setups
        Set_Mesh_Default_Options(opengl_SStri_mesh);

        ////Bind an initialized shader to the mesh
        opengl_SStri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

        ////Bind a texture to the mesh
        opengl_SStri_mesh->Clear_Textures();
        opengl_SStri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture("sails_tex"));
        opengl_SStri_mesh->Add_Texture("alpha", OpenGLTextureLibrary::Get_Texture("sails_alpha"));

        opengl_SStri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust1"));
        opengl_SStri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust2"));
        opengl_SStri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust3"));
        opengl_SStri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust4"));
        opengl_SStri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust5"));

        std::cout<<opengl_SStri_mesh->mesh.Elements().size()<<std::endl;
        ////Add the triangle mesh to the array to access the mesh later
        TriangleMesh<3>* triangle_mesh=&opengl_SStri_mesh->mesh;
        triangle_meshes.push_back(triangle_mesh);
    }


    /*
     * Initializes the ship body (without the sails)
     * Reads textures, etc.
     */
    void Init_ShipBody_Mesh()
    {
        ////Initialize the mesh file, shader, and texture of the mesh
        std::string mesh_file_name = "models/ship/Ship.obj";
        std::string shader_name = "ship";
        std::string texture_name = "ship_tex";
        std::string emissive_name = "ship_glow";
        std::string normal_name = "ship_norm";
        std::string spectral_name = "ship_spec";

        ////Read mesh from obj file
        OpenGLTriangleMesh* opengl_SBtri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
        std::vector<int> desired_shapes; desired_shapes.push_back(3);
        Read_Mesh(mesh_file_name, opengl_SBtri_mesh->mesh, desired_shapes);
        Rescale(opengl_SBtri_mesh->mesh.Vertices(),10.);

        ////Initialize the model matrix
        opengl_SBtri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        ////Other mesh initialization setups
        Set_Mesh_Default_Options(opengl_SBtri_mesh);

        ////Bind an initialized shader to the mesh
        opengl_SBtri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

        ////Bind a texture to the mesh

        opengl_SBtri_mesh->Clear_Textures();
        opengl_SBtri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture("ship_tex"));
        opengl_SBtri_mesh->Add_Texture("emissive", OpenGLTextureLibrary::Get_Texture("ship_glow"));
        opengl_SBtri_mesh->Add_Texture("normal", OpenGLTextureLibrary::Get_Texture("ship_norm"));
        opengl_SBtri_mesh->Add_Texture("spectral", OpenGLTextureLibrary::Get_Texture("ship_spec"));

        opengl_SBtri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust1"));
        opengl_SBtri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust2"));
        opengl_SBtri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust3"));
        opengl_SBtri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust4"));
        opengl_SBtri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust5"));

        std::cout<<opengl_SBtri_mesh->mesh.Elements().size()<<std::endl;
        ////Add the triangle mesh to the array to access the mesh later
        TriangleMesh<3>* triangle_mesh=&opengl_SBtri_mesh->mesh;
        triangle_meshes.push_back(triangle_mesh);
    }


	void Init_Plane_Mesh()
	{
		////Initialize the mesh file, shader, and texture of the mesh
		std::string shader_name = "sand";

		int width = 500;
		int height = 500;

		opengl_Ptri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
		////Create a mesh with vertices on a 500x500 lattice
		Create_Plane_Mesh(width,height,1.,&opengl_Ptri_mesh->mesh,0,2);
		Translate_Center_To(opengl_Ptri_mesh->mesh.Vertices(),Vector3(0.f, -10.f, 0.f));

		////Initialize the model matrix
		opengl_Ptri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		////Other mesh initialization setups
		Set_Mesh_Default_Options(opengl_Ptri_mesh);

		////Bind an initialized shader to the mesh
		opengl_Ptri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("sand"));
		opengl_Ptri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture("sand"));

		opengl_Ptri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust1"));
    opengl_Ptri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust2"));
    opengl_Ptri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust3"));
    opengl_Ptri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust4"));
    opengl_Ptri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust5"));

		////Add the triangle mesh to the array to access the mesh later
		TriangleMesh<3>* triangle_mesh=&opengl_Ptri_mesh->mesh;

		triangle_meshes.push_back(triangle_mesh);
	}

	void Init_Skybox()
	{
		std::string shader_name = "skybox";

		float size = 1.f;

		OpenGLTriangleMesh* opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
		std::vector<Vector3>& vertices = opengl_tri_mesh->mesh.Vertices();
		std::vector<Vector3i>& elements = opengl_tri_mesh->mesh.Elements();
		for (int f = 0; f < 108; f+=3) {
			vertices.push_back(Vector3(skyboxVertices[f]*size, skyboxVertices[f+1]*size, skyboxVertices[f+2]*size));
		}

		for (int f = 0; f < 36; f+=3) {
			elements.push_back(Vector3i(f, f+1, f+2));
		}

		////Initialize the model matrix
		opengl_tri_mesh->model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(100.f, 100.f, 100.f));

		////Other mesh initialization setups
		Set_Mesh_Default_Options(opengl_tri_mesh);

		////Bind an initialized shader to the mesh
		opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

		opengl_tri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture(shader_name));
	}

	void Sync_Simulation_And_Visualization_Data()
	{
		////if you want to animate your meshes or objects, implement this function
    //opengl_Ltri_mesh->model_matrix = glm::scale(opengl_Ltri_mesh->model_matrix, glm::vec3(1.000, 1.002, 1.000));

    // Move the character forwards
    time += .8;

    // Update mesh vertices
    int mode = (int)time % 16;

    printf("Got here\n");

    opengl_Ptri_mesh->Clear_Textures();
    opengl_Ptri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture("sand"));


    printf("za");
    opengl_BPtri_mesh->Clear_Textures();
    opengl_BPtri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture("sand"));

    printf("za");
    opengl_SStri_mesh->Clear_Textures();
    opengl_SStri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture("sails_tex"));
    opengl_SStri_mesh->Add_Texture("alpha", OpenGLTextureLibrary::Get_Texture("sails_alpha"));

    opengl_Ktri_mesh->Clear_Textures();
    opengl_Ktri_mesh->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture("kelp_tex"));
    opengl_Ktri_mesh->Add_Texture("normal", OpenGLTextureLibrary::Get_Texture("kelp_norm"));
    opengl_Ktri_mesh->Add_Texture("metallic", OpenGLTextureLibrary::Get_Texture("kelp_metal"));
    opengl_Ktri_mesh->Add_Texture("roughness", OpenGLTextureLibrary::Get_Texture("kelp_rough"));
    opengl_Ktri_mesh->Add_Texture("alpha", OpenGLTextureLibrary::Get_Texture("kelp_alpha"));
    opengl_Ktri_mesh->Add_Texture("emissive", OpenGLTextureLibrary::Get_Texture("kelp_rough"));

    printf("Got here0\n");
    Shift_Textures(opengl_BPtri_mesh, mode);
    printf("Got here1\n");
    Shift_Textures(opengl_Ptri_mesh, mode);
    printf("Got here2\n");

    Shift_Textures(opengl_SStri_mesh, mode);
    printf("Got here3\n");
    Shift_Textures(opengl_Ktri_mesh, mode);
  //  Shift_Textures(opengl_SBtri_mesh, mode);

  }

  void Shift_Textures(OpenGLTriangleMesh* tri_mesh, int mode) {

    printf("got here");
    switch(mode) {
      case 0:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust1"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust2"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust3"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust4"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust5"));

      break;

      case 1:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust2"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust3"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust4"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust5"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust6"));
      break;

      case 2:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust3"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust4"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust5"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust6"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust7"));
      break;

      case 3:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust4"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust5"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust6"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust7"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust8"));
      break;

      case 4:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust5"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust6"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust7"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust8"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust9"));
      break;

      case 5:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust6"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust7"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust8"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust9"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust10"));
      break;

      case 6:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust7"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust8"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust9"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust10"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust11"));
      break;

      case 7:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust8"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust9"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust10"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust11"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust12"));
      break;

      case 8:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust9"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust10"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust11"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust12"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust13"));
      break;

      case 9:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust10"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust11"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust12"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust13"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust14"));
      break;

      case 10:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust11"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust12"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust13"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust14"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust15"));
      break;

      case 11:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust12"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust13"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust14"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust15"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust16"));
      break;

      case 12:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust13"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust14"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust15"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust16"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust1"));
      break;

      case 13:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust14"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust15"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust16"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust1"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust2"));
      break;

      case 14:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust15"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust16"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust1"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust2"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust3"));
      break;

      case 15:
      tri_mesh->Add_Texture("caustic", OpenGLTextureLibrary::Get_Texture("caust16"));
      tri_mesh->Add_Texture("caustic2", OpenGLTextureLibrary::Get_Texture("caust1"));
      tri_mesh->Add_Texture("caustic3", OpenGLTextureLibrary::Get_Texture("caust2"));
      tri_mesh->Add_Texture("caustic4", OpenGLTextureLibrary::Get_Texture("caust3"));
      tri_mesh->Add_Texture("caustic5", OpenGLTextureLibrary::Get_Texture("caust4"));
      break;
    }

    tri_mesh->Set_Data_Refreshed();

    printf("time %d\n", time);
  }

	////update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	////Keyboard interaction
	virtual void Initialize_Common_Callback_Keys()
	{
		OpenGLViewer::Initialize_Common_Callback_Keys();
		Bind_Callback_Key('a',&Keyboard_Event_A_Func,"press A");
	}

	////Implement your own keyboard function,
	virtual void Keyboard_Event_A()
	{
		std::cout<<"A pressed"<<std::endl;
	}
	Define_Function_Object(TechProjDriver,Keyboard_Event_A);

protected:
	void Set_Mesh_Default_Options(OpenGLTriangleMesh* opengl_tri_mesh)
	{
		Set_Polygon_Mode(opengl_tri_mesh, PolygonMode::Fill);
		Set_Shading_Mode(opengl_tri_mesh, ShadingMode::Custom);
		opengl_tri_mesh->Set_Data_Refreshed();
		opengl_tri_mesh->Initialize();
	}

	//////////////////////////////////////////////////////////////////////////
	////Add a shader to the shader library
	void Add_Shader(const std::string vtx_shader_file_name,const std::string frag_shader_file_name,const std::string shader_name)
	{
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File(vtx_shader_file_name,frag_shader_file_name,shader_name);
	}

	//////////////////////////////////////////////////////////////////////////
	////Add a texture to the texture library
	void Add_Texture(const std::string texture_file_name,const std::string texture_name)
	{
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File(texture_file_name, texture_name);
	}

	void Add_Cube_Texture(const std::vector<std::string> texture_file_names, const std::string texture_file_name)
	{
		OpenGLTextureLibrary::Instance()->Add_Cube_Texture_From_File(texture_file_names, texture_file_name);
	}



  //////////////////////////////////////////////////////////////////////////
  ////Read an obj mesh
  void Read_Mesh(const std::string mesh_file_name,TriangleMesh<3>& read_mesh, std::vector<int> desired_shapes)
  	{
  		Array<std::shared_ptr<TriangleMesh<3>>> meshes;
  		Obj::Read_From_Obj_File(mesh_file_name, meshes);

          // Combine multiple shapes
          int num_vertices = 0;
          for (int& shape : desired_shapes/*= 0; shape < meshes.size(); shape++*/) {
              // Omit shape 2
              if (shape == 1) {
                  continue;
              }
              num_vertices = read_mesh.vertices->size();
              // Add the current shape's vertices, normals, and texture (uv) coords
              for (int j = 0; j < meshes[shape]->vertices->size(); j++) {
                  read_mesh.vertices->push_back((*meshes[shape]->vertices)[j]);
                  read_mesh.normals->push_back((*meshes[shape]->normals)[j]);
                  read_mesh.uvs->push_back((*meshes[shape]->uvs)[j]);
              }
              // Add the current shape's elements (faces)
              for (int j = 0; j < meshes[shape]->elements.size(); j++) {
                  Vector3i mesh_elements = meshes[shape]->elements[j];

                  read_mesh.elements.push_back(Vector3i(mesh_elements[0]+num_vertices,
                                                         mesh_elements[1]+num_vertices,
                                                         mesh_elements[2]+num_vertices));
              }
          }
  	}

	//////////////////////////////////////////////////////////////////////////
	////Initialize a plane mesh
	void Create_Plane_Mesh(const int m, const int n, const real dx, TriangleMesh<3>* mesh,int axis_0=0,int axis_1=1)
	{
		mesh->elements.resize(2*(m-1)*(n-1));
		int t=0;
		for(int i=1;i<=m-1;i++) for(int j=1;j<=n-1;j++) { // counterclockwise node ordering
			if(i%2){ //if i is even
				mesh->elements[t++] = Vector3i(i+m*(j-1),i+1+m*(j-1),i+m*j);
				mesh->elements[t++] = Vector3i(i+1+m*(j-1),i+1+m*j,i+m*j);
			}
			else {
				mesh->elements[t++]=Vector3i(i+m*(j-1),i+1+m*(j-1),i+1+m*j);
				mesh->elements[t++]=Vector3i(i+m*(j-1),i+1+m*j,i+m*j);
			}
		}
		for(size_type i=0;i<mesh->elements.size();i++){
			mesh->elements[i]-=Vector3i::Ones();
			/*swap y and z*/
			int tmp=mesh->elements[i][1];
			mesh->elements[i][1]=mesh->elements[i][2];
			mesh->elements[i][2]=tmp;
		}

		float yoff = 0;
		for(int j=0; j<n; j++) {
			float xoff = 0;
			for(int i=0;i<m;i++) {
			Vector3 pos=Vector3::Zero();
			pos[axis_0]=(real)i*dx;
			pos[axis_1]=(real)j*dx;

			unsigned int seed = 237;
			PerlinNoise pn(seed);

			//good wave variables: store for later: (real) 40*(pn.noise(pos[axis_0], pos[axis_1], .99f))-20
			pos[1] = (real) 30*(pn.noise(xoff, yoff, .5f))-15; //(real) noise[i][j]; //
			pos[1] += (real) 20*(pn.noise(pos[axis_0], pos[axis_1], .99f))-10;
			mesh->Vertices().push_back(pos);
			//printf("pos %f %f %f\n", pos[0], pos[1], pos[2]);

			xoff+=.04;
		}
		yoff+=.04;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	////Calculate the bounding box of a set of vertices
	void Bounding_Box(const Array<Vector3>& vertices,Vector3& min_corner,Vector3& max_corner)
	{
		min_corner=Vector3::Ones()*(1e7);max_corner=Vector3::Ones()*(-1e7);
		for(auto& v:vertices){
			min_corner=min_corner.cwiseMin(v);
			max_corner=max_corner.cwiseMax(v);}
	}

	//////////////////////////////////////////////////////////////////////////
	////Rescale the points to a box with longest_length
	void Rescale(Array<Vector3>& vertices,const real longest_length)
	{
		Vector3 min_corner,max_corner;
		Bounding_Box(vertices,min_corner,max_corner);
		Vector3 length=max_corner-min_corner;
		int axis=0;if(length[1]>length[axis])axis=1;if(length[2]>length[axis])axis=2;
		real rescale=(length[axis]>(real)0)?longest_length/length[axis]:(real)1;for(auto& v:vertices)v*=rescale;
	}

	//////////////////////////////////////////////////////////////////////////
	////Calculate the center
	Vector3 Center(const Array<Vector3>& vertices)
	{
		Vector3 sum=Vector3::Zero();for(auto& v:vertices)sum+=v;
		return sum/=(real)vertices.size();
	}

	//////////////////////////////////////////////////////////////////////////
	////Translate the center to the target position
	void Translate_Center_To(Array<Vector3>& vertices,const Vector3 target)
	{
		Vector3 center=Center(vertices);
		for(auto& v:vertices)v+=(target-center);
	}

	//////////////////////////////////////////////////////////////////////////
	////Initialize a sphere mesh
	void Create_Sphere_Mesh(const real r,/*rst*/TriangleMesh<3>* mesh,const int sub)
	{
		Initialize_Icosahedron_Mesh(r,mesh);for(int i=0;i<sub;i++)Subdivide(mesh);
		for(auto& v:mesh->Vertices()){real length=v.norm();real rs=r/length;v*=rs;}
	}

	void Initialize_Icosahedron_Mesh(const real scale,/*rst*/TriangleMesh<3>* mesh)
	{
		////http://donhavey.com/blog/tutorials/tutorial-3-the-icosahedron-sphere/
		const real tao=1.61803399f;
		real vtx_pos[12][3]={{1,tao,0},{-1,tao,0},{1,-tao,0},{-1,-tao,0},{0,1,tao},{0,-1,tao},{0,1,-tao},{0,-1,-tao},{tao,0,1},{-tao,0,1},{tao,0,-1},{-tao,0,-1}};
		int ele[20][3]={{0,1,4},{1,9,4},{4,9,5},{5,9,3},{2,3,7},{3,2,5},{7,10,2},{0,8,10},{0,4,8},{8,2,10},{8,4,5},{8,5,2},{1,0,6},{11,1,6},{3,9,11},{6,10,7},{3,11,7},{11,6,7},{6,0,10},{9,1,11}};

		mesh->Clear();
		int vtx_num=12;mesh->Vertices().resize(vtx_num);for(int i=0;i<vtx_num;i++){mesh->Vertices()[i]=Vector3(vtx_pos[i][0],vtx_pos[i][1],vtx_pos[i][2])*scale;}
		int ele_num=20;mesh->elements.resize(ele_num);for(int i=0;i<ele_num;i++)mesh->elements[i]=Vector3i(ele[i][0],ele[i][1],ele[i][2]);
	}

	void Subdivide(TriangleMesh<3>* mesh)
	{
		Hashtable<Vector2i,int> edge_vtx_hashtable;
		for(int k=0;k<mesh->elements.size();k++){
			for(int j=0;j<3;j++){
				Vector2i e(mesh->elements[k][j%3],mesh->elements[k][(j+1)%3]);
				if(e[0]<e[1])continue;
				Vector3 pos=(real).5*(mesh->Vertices()[e[0]]+mesh->Vertices()[e[1]]);
				mesh->Vertices().push_back(pos);
				int i=(int)mesh->Vertices().size()-1;
				edge_vtx_hashtable.insert(std::make_pair(e,i));
			}
		}

		auto n=mesh->elements.size();
		for(auto i=0;i<n;i++){const Vector3i v=mesh->elements[i];int v3,v4,v5;
			{auto search=edge_vtx_hashtable.find(Sorted(Vector2i(v[0],v[1])));if(search==edge_vtx_hashtable.end())continue;v3=search->second;}
			{auto search=edge_vtx_hashtable.find(Sorted(Vector2i(v[1],v[2])));if(search==edge_vtx_hashtable.end())continue;v4=search->second;}
			{auto search=edge_vtx_hashtable.find(Sorted(Vector2i(v[2],v[0])));if(search==edge_vtx_hashtable.end())continue;v5=search->second;}
			mesh->elements.push_back(Vector3i(v[0],v3,v5));
			mesh->elements.push_back(Vector3i(v3,v[1],v4));
			mesh->elements.push_back(Vector3i(v5,v4,v[2]));
			mesh->elements[i]=Vector3i(v3,v4,v5);}
	}
};

int main(int argc,char* argv[])
{
	int driver=1;

	switch(driver){
	case 1:{
		TechProjDriver driver;
		driver.Initialize();
		driver.Run();
	}break;
	}
}
