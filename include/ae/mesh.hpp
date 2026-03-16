#ifndef aeMesh
#define aeMesh

#include "ae/types.hpp"
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace ae { class Camera; }

namespace ae::gltf { struct GLTF; enum class Interpolation; }

namespace ae::anim
{

struct Keyframe
{
	f32 timestamp;
	glm::quat rotation;
};

struct Timeline
{
	u16 bone;
	ae::gltf::Interpolation func;
	std::vector<Keyframe> frames;
	usize currentFrame;
};

struct Animation
{
	f32 duration, currentTime;
	std::vector<Timeline> frames;
	bool repeat;
};

std::pair<std::string, Animation> loadAnimation(gltf::GLTF* file, u8 id);

}

namespace ae::mesh
{

class Bone
{
public:
	Bone();
	Bone(gltf::GLTF* file, u16 nodeID, u16* id);
	~Bone();
	void update(glm::mat4* ts, glm::mat3* f, glm::mat4 pts);
	u16 getID();
	f32 getLength();
	void render(glm::mat4* ts, glm::vec3* pts, u16* counter);
private:
	u16 nodeID, id;
	std::string name;
	std::vector<Bone> children;
	glm::mat3 angle;
	glm::mat4 translation;
	f32 length;
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();
	void load(gltf::GLTF* file, u8 id);
	void update(f32 dt, ae::Camera* cam);
	void render(ae::Camera* cam, glm::mat4 ts);
	void setAnimation(std::string name);
	void stopAnimation(std::string name);
private:
	glm::mat4* inverseBindMatrices;
	glm::mat4* ts;
	std::vector<Bone> bones;
	std::unordered_map<std::string, anim::Animation> anims;
	std::vector<std::string> currentAnim;
	u32 vbo;
	u16 bonesCount;
};

class Mesh
{
public:
	Mesh(ae::Camera* cam);
	~Mesh();
	void load(gltf::GLTF* file, const char* id);
	void destroy();
	void render(f32 dt, glm::mat3 rotation, glm::mat4 ts);
	Skeleton* getSkeleton();
private:
	u32 vbo;
	u32 ebo;
	usize indices;
	u32 texture;
	ae::Camera* cam;
	Skeleton* sk;
};

};

#endif