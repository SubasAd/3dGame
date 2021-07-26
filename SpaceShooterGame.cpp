#include "SpaceShooterGame.h"

#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include  "MathUtils.h"
#include "Material.h"
#include <time.h>
#include <iostream>

struct vertex
{
	Vector3D position;
	Vector2D texcoord;
};


__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	Vector4D m_light_direction;
	Vector4D m_camera_position;

	Vector4D m_light_position = Vector4D(0, 1, 0, 0);
	float m_light_radius = 4.0f;
	float m_time = 0.0f;

};


SpaceShooterGame::SpaceShooterGame()
{

	
}

void SpaceShooterGame::render()
{
	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);


	//RENDER SPACESHIP
	m_list_materials.clear();
	m_list_materials.push_back(m_spaceship_mat);
	updateModel(m_current_spaceship_pos, m_spaceship_rot, Vector3D(1, 1, 1), m_list_materials);

	drawMesh(m_spaceship_mesh, m_list_materials);
	



	

	
	

	//ASTEROIDS
    m_list_materials.clear();
	
	m_list_materials.push_back(m_asteroid_mat);

	
	

	for (unsigned int i = 0; i < 200; i++)
	{
		
	updateModel(m_asteroid_pos[i],m_asteroid_rot[i], m_asteroid_scale[i], m_list_materials);

	drawMesh(m_asteroid_mesh, m_list_materials);

	}


	

	//Render SKY
	if (m_spaceship_pos.m_z <= 9000) {
m_list_materials.clear();
	m_list_materials.push_back(m_sky_mat);
	drawMesh(m_sky_mesh, m_list_materials);
	}
	
		m_list_materials.clear();
		m_list_materials.push_back(m_finishing_sky_mat);
		updateModel(Vector3D(0, 0, 12000), Vector3D(1, 1, 1), Vector3D(3000, 3000, 3000), m_list_materials);
		drawMesh(m_finishing_sky_mesh, m_list_materials);
	
	

	//Render SKY
	



	m_swap_chain->present(true);


	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
	//m_delta_time = 1.0f / 30.0f;

	m_time += m_delta_time;
}

void SpaceShooterGame::update()
{
	updateSpaceship();
	//m_current_bullet_pos = m_current_spaceship_pos;
	updateThirdPersonCamera();
	updateLight();
	updateSkyBox();
	
	
}

void SpaceShooterGame::updateModel(Vector3D position, Vector3D rotation, Vector3D scale,const std::vector<MaterialPtr>& list_materials)
{
	constant cc;

	Matrix4x4 temp;
	cc.m_world.setIdentity();

	temp.setIdentity();
	temp.setScale(scale);

	cc.m_world *= temp;



	temp.setIdentity();
	temp.setRotationX(rotation.m_x);

	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(rotation.m_y);

	cc.m_world *= temp;


	temp.setIdentity();
	temp.setRotationZ(rotation.m_z);

	cc.m_world *= temp;


	temp.setIdentity();
	temp.setTranslation(position);

	cc.m_world *= temp;


	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;
	cc.m_camera_position = m_world_cam.getTranslation();


	cc.m_light_position = m_light_position;
	cc.m_light_radius =0.0f;
	cc.m_light_direction = m_light_rot_matrix.getZDirection();
	cc.m_time = m_time;


	for (size_t m = 0; m < list_materials.size(); m++)
	{
		list_materials[m]->setData(&cc, sizeof(constant));
		list_materials[m]->position = position;
	}

}

void SpaceShooterGame::updateCamera()
{
	Matrix4x4 world_cam, temp;
	world_cam.setIdentity();

	m_cam_rot.m_x += m_delta_mouse_y * m_delta_time * 0.1f;
	m_cam_rot.m_y += m_delta_mouse_x * m_delta_time * 0.1f;



	temp.setIdentity();
	temp.setRotationX(m_cam_rot.m_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_cam_rot.m_y);
	world_cam *= temp;

	Vector3D new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (m_forward * 0.05f);

	new_pos = new_pos + world_cam.getXDirection() * (m_rightward * 0.05f);

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	m_view_cam = world_cam;

	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_proj_cam.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);
}

void SpaceShooterGame::updateThirdPersonCamera()
{

	Matrix4x4 world_cam, temp;
	world_cam.setIdentity();

	m_cam_rot.m_x +=m_delta_mouse_y *m_delta_time * 0.1f;
	m_cam_rot.m_y += m_delta_mouse_x *m_delta_time * 0.1f;

	if (m_cam_rot.m_x >= 1.57f)  m_cam_rot.m_x = 1.57f;
	else if (m_cam_rot.m_x <= -1.57f)  m_cam_rot.m_x = -1.57f;


	m_current_cam_rot = Vector3D::lerp(m_current_cam_rot, m_cam_rot, 3.0f * m_delta_time);


	temp.setIdentity();
	temp.setRotationX(m_current_cam_rot.m_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_current_cam_rot.m_y);
	world_cam *= temp;

	

	m_cam_pos = m_current_spaceship_pos;


	if (m_forward)
	{

		if (m_turbo_mode)
		{

			if (m_forward > 0.0f) m_cam_distance = 30.0f;
			else m_cam_distance = 5.0f;


		}
		else
		{
if (m_forward > 0.0f)  m_cam_distance = 16.0f;
		
		else m_cam_distance = 9.0f;
		}
		
		



	}
	else
	{
		m_cam_distance = 14.0f;

	}

	 
	m_current_cam_distance = lerp(m_current_cam_distance, m_cam_distance, 2.0f * m_delta_time);



	Vector3D new_pos = m_cam_pos + world_cam.getZDirection() * (-m_current_cam_distance);
	new_pos = new_pos + world_cam.getYDirection() * (5.0f);

	

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	m_view_cam = world_cam;

	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_proj_cam.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 9000.0f);


}

void SpaceShooterGame::updateSkyBox()
{
	constant cc;

	cc.m_world.setIdentity();
	cc.m_world.setScale(Vector3D(8000.0f, 8000.0f, 8000.0f));

	cc.m_world.setTranslation(m_world_cam.getTranslation());
	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;
	m_sky_mat->setData(&cc, sizeof(constant));
	//m_finishing_sky_mat->setData(&cc, sizeof(constant));

}
void SpaceShooterGame::updateLight()
{

	Matrix4x4 temp;
	temp.setIdentity();
	m_light_rot_matrix.setIdentity();


	temp.setIdentity();
	temp.setRotationX(-0.707f);
	m_light_rot_matrix *= temp;



	temp.setIdentity();
	temp.setRotationY(0.707f);
	m_light_rot_matrix *= temp;
}




void SpaceShooterGame::updateSpaceship()
{
	Matrix4x4 world_model, temp;
	world_model.setIdentity();

	m_spaceship_rot.m_x += m_delta_mouse_y * m_delta_time * 0.1f;
	m_spaceship_rot.m_y += m_delta_mouse_x * m_delta_time * 0.1f;

	if (m_spaceship_rot.m_x >= 1.57f)  m_cam_rot.m_x = 1.57f;
	else if (m_spaceship_rot.m_x <= -1.57f)  m_cam_rot.m_x = -1.57f;

m_current_spaceship_rot = Vector3D::lerp(m_current_spaceship_rot, m_spaceship_rot, 5.0f * m_delta_time);

	temp.setIdentity();
	temp.setRotationX(m_current_spaceship_rot.m_x);
	world_model *= temp;

	temp.setIdentity();
	temp.setRotationY(m_current_spaceship_rot.m_y);
	world_model *= temp;

	
	m_spaceship_speed ;
	if (m_turbo_mode) {
		m_spaceship_speed =350.0f;

	}
	else
	{
		m_spaceship_speed = m_normal_speed;
	}
	
	

	bool correctspaceshippos = (m_current_spaceship_pos.m_x <= 9000 || m_current_spaceship_pos.m_x >= -9000) && (m_current_spaceship_pos.m_y <= 9000 || m_current_spaceship_pos.m_y >= -9000) && (m_current_spaceship_pos.m_z <= 9000 || m_current_spaceship_pos.m_z >= -9000);


	if (correctspaceshippos)
	{

		m_spaceship_pos = m_spaceship_pos + world_model.getZDirection() * (m_forward)*m_spaceship_speed * m_delta_time;
      m_current_spaceship_pos = Vector3D::lerp(m_current_spaceship_pos, m_spaceship_pos,3.0f*m_delta_time );
	  m_spaceship_pos = m_spaceship_pos + world_model.getXDirection() * (m_rightward) * 125.0f * m_delta_time;
	}
	else
	{
		m_current_spaceship_pos = m_spaceship_pos;
	}
	std::cout << m_spaceship_pos.m_z << "   ";

	
	if (m_spaceship_pos.m_x >= 100  )
	
	{
		m_spaceship_pos = Vector3D(100, m_spaceship_pos.m_y, m_spaceship_pos.m_z);
		m_current_spaceship_pos = Vector3D(100, m_spaceship_pos.m_y, m_spaceship_pos.m_z);
		std::cout <<"m_current_spaceship_pos" << m_current_spaceship_pos.m_x << std::endl;
	}
	if (m_spaceship_pos.m_x <= -100)

	{
		m_spaceship_pos = Vector3D(-100, m_spaceship_pos.m_y, m_spaceship_pos.m_z);
		m_current_spaceship_pos = Vector3D(-100, m_spaceship_pos.m_y, m_spaceship_pos.m_z);
		std::cout << "m_current_spaceship_pos" << m_current_spaceship_pos.m_x << std::endl;
	}
	
	if (m_spaceship_pos.m_y >= 100)

	{
		m_spaceship_pos = Vector3D( m_spaceship_pos.m_x, 100,m_spaceship_pos.m_z);
		m_current_spaceship_pos = Vector3D( m_spaceship_pos.m_y,100, m_spaceship_pos.m_z);
		std::cout << "m_current_spaceship_pos" << m_current_spaceship_pos.m_y << std::endl;
	}
	if (m_spaceship_pos.m_y <= -100)

	{
		m_spaceship_pos = Vector3D( m_spaceship_pos.m_y,-100, m_spaceship_pos.m_z);
		m_current_spaceship_pos = Vector3D(m_spaceship_pos.m_y,-100, m_spaceship_pos.m_z);
		std::cout << "m_current_spaceship_pos" << m_current_spaceship_pos.m_x << std::endl;
	}
	if (m_spaceship_pos.m_z >= 9000)
	{
		m_spaceship_speed = 0.0f;
		m_normal_speed = 0.0f;
	}
	if (m_spaceship_pos.m_z >= 12000)

	{
		
		m_spaceship_pos = Vector3D(m_spaceship_pos.m_x, m_spaceship_pos.m_y,90000);
		m_current_spaceship_pos = Vector3D(m_spaceship_pos.m_y, m_spaceship_pos.m_y, m_spaceship_pos.m_z);
		std::cout << "m_current_spaceship_pos" << m_current_spaceship_pos.m_y << std::endl;
	}
	

}

void SpaceShooterGame::drawMesh(const MeshPtr& mesh, const std::vector<MaterialPtr>& list_materials)
{
	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(mesh->getVertexBuffer());
	//SET THE INDICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(mesh->getIndexBuffer());


	for (size_t m = 0; m < mesh->getNumMaterialSlots(); m++)
	{

		if (m >= list_materials.size()) break;

		MaterialSlot mat = mesh->getMaterialSlot(m);

		GraphicsEngine::get()->setMaterial(list_materials[m]);
		// FINALLY DRAW THE TRIANGLE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mat.num_indices, 0, mat.start_index);
 
	}

}

SpaceShooterGame::~SpaceShooterGame()
{
}

void SpaceShooterGame::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);

	m_play_state = true;
	InputSystem::get()->showCursor(false);

    RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);


	srand((unsigned int) time(NULL));

	for (unsigned int i = 0; i < 200; i++)
	{
		m_asteroid_pos[i] = Vector3D(rand()%1800+(-900), rand() % 1800 + (-900), rand() % 1800 + (-900));

		m_asteroid_rot[i] = Vector3D((rand() % 628)/100.0f , (rand() % 628) / 100.0f, (rand() % 628) / 100.0f);
		float scale = (rand() % 20 + (6));

		m_asteroid_scale[i] = Vector3D(scale,scale,scale);


	}



	m_sky_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\stars_map.jpg");
	m_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\sphere.obj");
	
	m_bullet_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\bullet.jpg");
	m_bullet_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\Spaceshipbullet.obj");

	m_spaceship_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\spaceship.jpg");
	m_spaceship_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\spaceship.obj");

	m_asteroid_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\asteroid.jpg");
	m_asteroid_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\asteroid.obj");


	m_finishing_sky_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\sky.jpg");
	m_finishing_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\sphere.obj");

	m_asteroid_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\asteroid.jpg");
	m_asteroid_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\asteroid.obj");



	m_base_mat = GraphicsEngine::get()->createMaterial(L"DirectionalLightVertexShader.hlsl", L"DirectionalLightPixelShader.hlsl");
	m_base_mat->setCullMode(CULL_MODE_BACK);

	m_spaceship_mat = GraphicsEngine::get()->createMaterial(m_base_mat);
	m_spaceship_mat->addTexture(m_spaceship_tex);
	m_spaceship_mat->setCullMode(CULL_MODE_BACK);

	m_asteroid_mat = GraphicsEngine::get()->createMaterial(m_base_mat);
	m_asteroid_mat->addTexture(m_asteroid_tex);
	m_asteroid_mat->setCullMode(CULL_MODE_BACK);


	m_sky_mat = GraphicsEngine::get()->createMaterial(L"SkyBoxVertexShader.hlsl", L"SkyBoxPixelShader.hlsl");
	m_sky_mat->addTexture(m_sky_tex);
	m_sky_mat->setCullMode(CULL_MODE_FRONT);

	m_finishing_sky_mat = GraphicsEngine::get()->createMaterial(m_base_mat);
	m_finishing_sky_mat->addTexture(m_finishing_sky_tex);
	m_finishing_sky_mat->setCullMode(CULL_MODE_FRONT);

	m_bullet_mat = GraphicsEngine::get()->createMaterial(m_base_mat);
	m_bullet_mat->addTexture(m_bullet_tex);

	m_world_cam.setTranslation(Vector3D(0, 0, -2));
    m_list_materials.reserve(32);
}

void SpaceShooterGame::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();

	//COMPUTE TRANSFORM MATRICES
	this->update();
	this->checkCollision();
	this->render();
	
	
}

void SpaceShooterGame::onDestroy()
{
	Window::onDestroy();
	m_swap_chain->setFullScreen(false, 1, 1);
}

void SpaceShooterGame::onFocus()
{
	InputSystem::get()->addListener(this);
}

void SpaceShooterGame::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void SpaceShooterGame::onSize()
{
	RECT rc = this->getClientWindowRect();
	m_swap_chain->resize(rc.right-rc.left, rc.bottom - rc.top);
	this->render();
}

void SpaceShooterGame::onKeyDown(int key)
{
	if (!m_play_state) return;

	if (key == 'W')
	{
		//m_rot_x += 3.14f*m_delta_time;
		m_forward = 1.0f;
	}
	else if (key == 'S')
	{
		//m_rot_x -= 3.14f*m_delta_time;
		m_forward = -1.0f;
	}
	else if (key == 'A')
	{
		//m_rot_y += 3.14f*m_delta_time;
		m_rightward = -1.0f;
	}
	else if (key == 'D')
	{
		//m_rot_y -= 3.14f*m_delta_time;
		m_rightward = 1.0f;
	}
	
	else if (key == VK_SHIFT)
	{
		
			m_turbo_mode = true;
		

		//m_rot_y -= 3.14f*m_delta_time;
		


	}
	else if (key == VK_SPACE)
	{

	
		updateModel(m_current_spaceship_pos, m_current_spaceship_rot, Vector3D(1, 1, 1), m_list_materials);

		

	}


	

}

void SpaceShooterGame::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;

	if (key == VK_ESCAPE)
	{
		if (m_play_state) {
                      m_play_state =  false ;
	                	InputSystem::get()->showCursor(!m_play_state);
		 }
		
	}
	else if (key == 'F')
	{
		m_fullscreen_state = (m_fullscreen_state) ? false : true;
		RECT size_screen = this->getSizeScreen();

		m_swap_chain->setFullScreen(m_fullscreen_state, size_screen.right, size_screen.bottom);
	}

	else if (key == VK_SHIFT)
	{
		//m_rot_y -= 3.14f*m_delta_time;
		m_turbo_mode = false;
	}
	else if (key == 'I')
	{
		m_spaceship_pos = Vector3D(0,0,0);
	}

}

void SpaceShooterGame::onMouseMove(const Point& mouse_pos)
{
	if (!m_play_state) return;

	RECT win_size = this->getClientWindowRect();


	int width = (win_size.right - win_size.left);
	int height = (win_size.bottom - win_size.top);  

	m_delta_mouse_x =(int) (mouse_pos.m_x - (int)(win_size.left + (width / 2.0f)));
	m_delta_mouse_y = (int)(mouse_pos.m_y - (int)(win_size.top + (height / 2.0f)));

	


	InputSystem::get()->setCursorPosition(Point(win_size.left +(int)(width / 2.0f), win_size.top + (int)(height / 2.0f)));

}

void SpaceShooterGame::onLeftMouseDown(const Point& mouse_pos)
{
	if (!m_play_state) {
		m_play_state = true;
		InputSystem::get()->showCursor(!m_play_state);
	}
	
}

void SpaceShooterGame::onLeftMouseUp(const Point& mouse_pos)
{
	
}

void SpaceShooterGame::onRightMouseDown(const Point& mouse_pos)
{
	
}

void SpaceShooterGame::onRightMouseUp(const Point& mouse_pos)
{
	
}
bool SpaceShooterGame::checkCollision()
{
	
	for (unsigned int i = 0; i < 200; i++)
	{
		Vector3D* dist =new Vector3D( m_spaceship_pos - m_asteroid_pos[i]);
		float magnitude = pow((dist->m_x *dist-> m_x +dist-> m_y *dist-> m_y +dist-> m_z * dist->m_z), 0.5);
		
		if (m_asteroid_pos[i].m_z - m_spaceship_pos.m_z <= -25  )
		{

			if (m_asteroid_pos[i].m_z == 0) {}
			m_asteroid_pos[i]   = Vector3D(0,0, m_current_spaceship_pos.m_z)+ Vector3D(0, 0, 90) + m_asteroid_pos[i];
			//render();
		}


		Vector3D* scale = new Vector3D(m_asteroid_scale[i]);
		float scalemagn = pow((scale->m_x * scale->m_x + scale->m_y * scale->m_y + scale->m_z * scale->m_z), 0.5);
		//std::cout << "Not Collided" << magnitude;
		if (magnitude <= scalemagn*2.7-6)
		{
			
			
			
		m_spaceship_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\house.obj");
		if (m_delta_time <= 400)
		{
			m_spaceship_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\spaceship.obj");
		}
		
		

		}

	}


	

	

	return false;
	
}
void SpaceShooterGame::createAsteroid()
{

	//m_asteroid_pos[0] = Vector3D(0, 0, 0);

}
