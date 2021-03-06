#pragma once
#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputListener.h"
#include "Matrix4x4.h"

class SpaceShooterGame : public Window, public InputListener
{
  public:
	SpaceShooterGame();
	~SpaceShooterGame();

	// Inherited via Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;
	virtual void onSize() override;


	// Inherited via InputListener
	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;
	virtual void onMouseMove(const Point& mouse_pos) override;

	virtual void onLeftMouseDown(const Point& mouse_pos) override;
	virtual void onLeftMouseUp(const Point& mouse_pos) override;
	virtual void onRightMouseDown(const Point& mouse_pos) override;
	virtual void onRightMouseUp(const Point& mouse_pos) override;
	bool checkCollision();
public:
	void render();
	void update();
	void updateModel(Vector3D position, Vector3D rotation, Vector3D scale, const std::vector<MaterialPtr>& list_materials);


	void updateCamera();
	void updateThirdPersonCamera();
	void updateSkyBox();
	void updateLight();
	
	void updateSpaceship();
	void drawMesh(const MeshPtr& mesh, const std::vector<MaterialPtr>& list_materials);
	void createAsteroid();

private:
	SwapChainPtr m_swap_chain;
	

	TexturePtr m_spaceship_tex;
	MeshPtr m_spaceship_mesh;
	MaterialPtr m_spaceship_mat;


	TexturePtr m_asteroid_tex;
	MeshPtr m_asteroid_mesh;
	MaterialPtr m_asteroid_mat;

	
	TexturePtr m_damaged_asteroid_tex;
	MeshPtr m__damaged_asteroid_mesh;
	MaterialPtr m_damaged_asteroid_mat;

	TexturePtr m_bullet_tex;
	MeshPtr m_bullet_mesh;
	MaterialPtr m_bullet_mat;


	TexturePtr m_finishing_sky_tex;
	MeshPtr m_finishing_sky_mesh;
	MaterialPtr m_finishing_sky_mat;



	TexturePtr m_sky_tex;
	MeshPtr m_sky_mesh;
	MaterialPtr m_sky_mat;

	MaterialPtr m_base_mat;

private:
	long m_old_delta;
	long m_new_delta;
	float m_delta_time;


	


	float m_current_cam_distance ;


    float m_cam_distance   = 14.0f;
	Vector3D m_cam_rot;
	Vector3D m_cam_pos;

	Vector3D m_current_cam_rot;
	//Vector3D m_current_cam_pos;

	float m_spaceship_speed = 250.0f;
	float m_normal_speed = 250.0f;
	float m_turbo_counter = 0;



	Vector3D m_current_spaceship_pos;



	Vector3D m_spaceship_pos;
	//Vector3D m_current_bullet_pos = m_spaceship_pos;

	Vector3D m_current_spaceship_rot;

	Vector3D m_spaceship_rot;

	float m_delta_mouse_x = 0.0f, m_delta_mouse_y = 0.0f;

	Vector3D m_asteroid_pos[200];
	Vector3D m_asteroid_rot[200];
	Vector3D m_asteroid_scale[200];
	
	float m_forward = 0.0f;
	float m_rightward = 0.0f;

	bool m_turbo_mode  = false;


	bool cull = true;
	Matrix4x4 m_world_cam;
	Matrix4x4 m_view_cam;
	Matrix4x4 m_proj_cam;

	float m_time = 0.0f;
	
	bool m_anim = false;


	
	Matrix4x4 m_light_rot_matrix;

	bool m_play_state = false;
	bool m_fullscreen_state = false;






	std::vector<MaterialPtr> m_list_materials;
	Vector4D m_light_position;
};

