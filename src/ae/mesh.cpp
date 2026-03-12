#include <ae/mesh.hpp>
#include <ae/camera.hpp>
#include <ae/global.hpp>

#include <glad/glad.h>

ae::mesh::Mesh::Mesh(ae::Camera* camera)
{
	if (camera == nullptr) return;
	this->vbo = camera->createVBO();
	this->ebo = camera->createVBO();
	this->cam = camera;
	this->texture = 0;
}

ae::mesh::Mesh::~Mesh()
{
	this->vbo = 0;
	this->ebo = 0;
	delete this->sk;
}

void ae::mesh::Mesh::load(ae::gltf::GLTF* file, const char* id)
{
	gltf::Node* node = nullptr;
	for (auto x: file->nodes) if (x.name == id) { node = &x; break; }
	if (!node)
	{
		printf("Error: Node \"%s\" not found\n", id);
		return;
	}
	if (node->mesh == U8_MAX)
	{
		printf("Error: Node \"%s\" does not contain mesh\n", id);
		return;
	}

	auto m = &file->meshes[node->mesh];
	printf("Loading node \"%s\" as mesh \"%s\"\n", id, m->name.c_str());

	auto ia = &file->accessors[m->indices];
	auto ibv = &file->bufferViews[ia->bufferView];
	auto ib = &file->buffers[ibv->buffer];

	auto va = &file->accessors[m->vertices];
	auto vbv = &file->bufferViews[va->bufferView];
	auto vertices = (glm::vec3*)&file->buffers[vbv->buffer]
		.data[vbv->byteOffset];

	auto na = &file->accessors[m->normal];
	auto nbv = &file->bufferViews[na->bufferView];
	auto normals = (glm::vec3*)&file->buffers[nbv->buffer]
		.data[nbv->byteOffset];

	auto ta = &file->accessors[m->texCoord];
	auto tbv = &file->bufferViews[ta->bufferView];
	auto uvs = (glm::vec2*)&file->buffers[tbv->buffer]
		.data[tbv->byteOffset];

	auto ja = &file->accessors[m->joints];
	auto jbv = &file->bufferViews[ja->bufferView];
	auto joints = (glm::u8vec4*)&file->buffers[jbv->buffer]
		.data[jbv->byteOffset];

	auto buf = new f32[va->count * 9];

	for (usize i = 0 ; i < va->count; i++)
	{
		(*(glm::vec3*)&buf[i*9+0]) = vertices[i];
		(*(glm::vec3*)&buf[i*9+3]) = normals[i];
		buf[i*9+6] = (f32)joints[i].x;
		(*(glm::vec2*)&buf[i*9+7]) = uvs[i];
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(
		GL_ARRAY_BUFFER, va->count * 9 * sizeof(f32),
		buf, GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, ibv->byteLength,
		&ib->data[ibv->byteOffset], GL_STATIC_DRAW
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	this->indices = ia->count;

	if (node->skin != U8_MAX)
	{
		this->sk = new Skeleton;
		this->sk->load(file, node->skin);
	}

	if (m->material == U8_MAX) return;

	auto mat = &file->materials[m->material];
	printf("Using material \"%s\"\n", mat->name.c_str());
	auto tex = &file->textures[mat->texture];
	auto img = &file->images[tex->source];
	auto sampler = &file->samplers[tex->sampler];

	this->texture = this->cam->getTexture(
		ae::str::format("meshes/%s", img->name.c_str()).c_str()
	).id;
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler->magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler->magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler->wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler->wrapT);
}

void ae::mesh::Mesh::destroy()
{
	this->cam->removeVBO(this->vbo);
	this->cam->removeVBO(this->ebo);
	glDeleteTextures(1, &this->texture);
	if (this->sk != nullptr) delete this->sk;
	this->cam = nullptr;
}

void ae::mesh::Mesh::render(f32 dt, glm::mat3 rotation, glm::mat4 ts)
{
	this->cam->shaderUse("mesh");
	if (this->sk != nullptr) this->sk->update(dt, this->cam);
	this->cam->bindMeshVAO();
	this->cam->bindTexture(this->texture);
	this->cam->shaderMat3("rotation", rotation);
	this->cam->shaderSetModel(ts);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 36, 0
	);
	glVertexAttribPointer(
		1, 4, GL_FLOAT, GL_FALSE, 36, (void*)12
	);
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE, 36, (void*)28
	);
	glDrawElements(GL_TRIANGLES, this->indices, GL_UNSIGNED_SHORT, 0);
}

ae::mesh::Skeleton* ae::mesh::Mesh::getSkeleton()
{
	return this->sk;
}

ae::f32 ae::anim::applyInterpolation(
	ae::anim::Interpolation ip, f32 t
)
{
	using IP = ae::anim::Interpolation;
	switch (ip)
	{
		case IP::Step: return 0;
		case IP::Linear: return t;
		case IP::CubicSpline: return t < 0.5 ?
		(4 * pow(t, 3)) :
		(1 - pow(-2 * t + 2, 3) * 0.5);
	}
	return 0;
}

std::tuple<std::string, ae::anim::Animation>
	ae::anim::loadAnimation(gltf::GLTF *file, u8 id)
{
	auto anim = &file->animations[id];
	Animation a;
	a.currentTime = 0.0;
	a.duration = anim->duration;
	a.repeat = anim->repeat;
	for (auto c: anim->channels)
	{
		// TODO if you'll add ability to change smth other than rotation,
		// then use timeline not as vector of keyframes,
		// but number of vectors for each type of changes
		using TP = gltf::Animation::Channel::TargetPath;
		if (c.targetPath != TP::Rotation) { continue; }
		Timeline tl;
		auto s = &anim->samplers[c.sampler];
		tl.func = s->interpolation;
		tl.currentFrame = 0;
		tl.bone = c.targetNode;

		auto ia = &file->accessors[s->input];
		auto ibv = &file->bufferViews[ia->bufferView];
		auto ts = (f32*)&file->buffers[ibv->buffer].data[ibv->byteOffset];
		
		auto oa = &file->accessors[s->output];
		auto obv = &file->bufferViews[oa->bufferView];
		auto rot = (glm::vec4*)&file->buffers[obv->buffer].data[obv->byteOffset];

		for (u16 i = 0; i < ia->count; i++)
		{
			auto r = rot[i * 3 + 1];
			tl.frames.push_back({
				.timestamp = ts[i],
				.rotation = glm::quat(r.w, r.x, r.y, r.z)
			});
		}

		a.frames.push_back(tl);
	}
	return {anim->name, a};
}

ae::mesh::Bone::Bone()
{
	this->children = {};
	this->angle = glm::mat3(1.0);
	this->translation = glm::mat4(1.0);
	this->length = 1.0;
	this->id = U16_MAX;
}

ae::mesh::Bone::Bone(gltf::GLTF* file, u16 id)
{
	auto n = &file->nodes[id];
	
	this->name = n->name;
	this->id = id;
	this->angle = glm::mat3_cast(n->rotation);
	this->translation = glm::translate(glm::mat4(1.0), n->translation);

	if (!n->children.empty())
	{
		this->length = glm::length(file->nodes[n->children[0]].translation);
	}
	else { this->length = 0.5; }

	this->children = n->children;
	
	printf("%s %i\n",
		n->name.c_str(), this->id
	);
}

ae::mesh::Bone::~Bone()
{
	this->children.clear();
}

void ae::mesh::Bone::update(
	glm::mat4* bones, glm::mat3* frame, glm::mat4* ts, glm::mat3* joints,
	u16 index
)
{
	if (bones[this->id] == glm::mat4(1.0)) bones[this->id] = this->translation;
	glm::mat4 parent = bones[this->id];
	glm::mat3 state = frame[this->id];

	ts[index] = parent * glm::mat4(state);
	joints[index] = state * joints[index];
	glm::mat4 t = glm::translate(glm::mat4(1.0), glm::vec3(0, this->length, 0));

	for (u16 i: this->children)
	{
		bones[i] = ts[index] * t;
		joints[i] = joints[index];
	}
}

ae::u16 ae::mesh::Bone::getID() { return this->id; }
ae::f32 ae::mesh::Bone::getLength() { return this->length; }

ae::mesh::Skeleton::Skeleton()
{
	this->inverseBindMatrices = nullptr;
	this->ts = nullptr;
	this->joints = nullptr;
	this->bones = {};
	this->anims = {};
	this->currentAnim = std::string();
	glGenBuffers(1, &this->vbo);
}

ae::mesh::Skeleton::~Skeleton()
{
	delete[] this->inverseBindMatrices;
	delete[] this->ts;
	delete[] this->joints;
	glDeleteBuffers(1, &this->vbo);
}

void ae::mesh::Skeleton::load(gltf::GLTF* file, u8 id)
{
	auto skin = &file->skins[id];

	auto ma = &file->accessors[skin->inverseBindMatrices];
	auto mbv = &file->bufferViews[ma->bufferView];
	auto ibm = (glm::mat4*)&file->buffers[mbv->buffer].data[mbv->byteOffset];
	
	this->ts = new glm::mat4[skin->joints.size()];
	this->joints = new glm::mat3[skin->joints.size()];
	this->inverseBindMatrices = new glm::mat4[skin->joints.size()];
	memcpy(this->inverseBindMatrices, ibm, skin->joints.size() * sizeof(glm::mat4));
	
	this->bones.resize(skin->joints.size());
	for (usize i = 0; i < skin->joints.size(); i++)
	{
		this->bones[i] = Bone(file, skin->joints[i]);
	}

	for (u8 i = 0; i < file->animations.size(); i++)
	{
		auto [
			name, anim
		] = anim::loadAnimation(file, i);
		this->anims.insert({name, anim});
	}
}

void ae::mesh::Skeleton::update(f32 dt, ae::Camera* cam)
{
	auto frame = new glm::mat3[this->bones.size()];
	auto bones = new glm::mat4[this->bones.size()];

	for (usize i = 0; i < this->bones.size(); i++)
	{
		bones[i] = glm::mat4(1.0);
		frame[i] = glm::mat3(1.0);
	}
	memcpy(this->ts, bones, this->bones.size() * sizeof(glm::mat4));
	memcpy(this->joints, frame, this->bones.size() * sizeof(glm::mat3));

	if (!this->currentAnim.empty())
	{
		auto anim = &this->anims.at(this->currentAnim);
		anim->currentTime += dt;
		while (anim->currentTime > anim->duration)
		{
			if (anim->repeat) anim->currentTime -= anim->duration;
			else anim->currentTime = anim->duration;
		}

		for (auto& tl: anim->frames)
		{
			if (tl.frames.size() == 1)
			{
				frame[tl.bone] = glm::mat3_cast(tl.frames[0].rotation);
				continue;
			}
			if (anim->currentTime == anim->duration)
			{
				frame[tl.bone] = glm::mat3_cast(tl.frames[tl.frames.size() - 1].rotation);
				continue;
			}

			auto f1 = &tl.frames[tl.currentFrame];
			auto f2 = &tl.frames[tl.currentFrame + 1];

			if (f1->timestamp > anim->currentTime)
			{
				tl.currentFrame = 0;
				f1 = &tl.frames[tl.currentFrame];
				f2 = &tl.frames[tl.currentFrame + 1];
			}

			if (f2->timestamp <= anim->currentTime)
			{
				tl.currentFrame++;
				f1 = &tl.frames[tl.currentFrame];
				f2 = &tl.frames[tl.currentFrame + 1];
			}

			auto t = (anim->currentTime - f1->timestamp) / (f2->timestamp - f1->timestamp);
			frame[tl.bone] = glm::mat3_cast(glm::slerp(
				f1->rotation, f2->rotation,
				ae::anim::applyInterpolation(tl.func, t)
			));
		}
	}

	for (usize i = 0; i < this->bones.size(); i++)
	{
		this->bones[i].update(bones, frame, this->ts, this->joints, i);
	}

	auto ibm = cam->shaderGetPos("ibm");
	auto ts = cam->shaderGetPos("bones");
	auto joints = cam->shaderGetPos("joints");
	glUniformMatrix4fv(
		ibm, this->bones.size(),
		GL_FALSE, (f32*)this->inverseBindMatrices
	);
	glUniformMatrix4fv(
		ts, this->bones.size(),
		GL_FALSE, (f32*)this->ts
	);
	glUniformMatrix3fv(
		joints, this->bones.size(),
		GL_FALSE, (f32*)this->joints
	);

	delete[] frame;
	delete[] bones;
}

void ae::mesh::Skeleton::render(ae::Camera* cam, glm::mat4 ts)
{
	auto pts = new glm::vec3[this->bones.size() * 2];
	for (usize i = 0; i < this->bones.size(); i++)
	{
		pts[i*2+0] = this->ts[i] * glm::vec4(0, 0, 0, 1);
		pts[i*2+1] = this->ts[i] * glm::vec4(0, this->bones[i].getLength(), 0, 1);
	}
	cam->shaderUse("skeleton");
	cam->bindSkeletonVAO();
	cam->shaderSetModel(ts);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(
		GL_ARRAY_BUFFER, this->bones.size() * 2 * sizeof(glm::vec3),
		pts, GL_STATIC_DRAW
	);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 12, 0
	);
	glDepthFunc(GL_ALWAYS);
	glDrawArrays(GL_LINES, 0, this->bones.size() * 2);
	glDepthFunc(GL_LESS);
}

void ae::mesh::Skeleton::setAnimation(std::string name)
{
	auto t = this->anims.find(name);
	if (t == this->anims.end()) return;
	this->currentAnim = name;
}

ae::gltf::GLTF* ae::gltf::load(const char* id)
{
	auto src = ae::fs::readJSON(ae::str::format(
		"res/meshes/%s.gltf", id
	));
	if (src.empty())
	{
		printf("Error while loading GLTF \"%s\": file is empty\n", id);
		return nullptr;
	}

	auto f = new GLTF;

	for (auto x: src["buffers"])
	{
		auto uri = ae::str::format("res/meshes/%s", x["uri"].asCString());
		auto len = x["byteLength"].asUInt();
		auto [length, data] = ae::fs::readBinary(uri);
		if (len != length)
		{
			printf("WARNING: GLTF \"%s\": Lengths are not the same(%i|%u)\n",
				x["uri"].asCString(), len, length
			);
		}
		f->buffers.push_back({
			.data = data,
			.byteLength = length
		});
	}

	for (auto x: src["bufferViews"])
	{
		f->bufferViews.push_back({
			.buffer = (u8)x["buffer"].asUInt(),
			.byteOffset = x["byteOffset"].asUInt(),
			.byteLength = x["byteLength"].asUInt(),
			.target = (u16)x["target"].asUInt()
		});
	}

	for (auto x: src["accessors"])
	{
		f->accessors.push_back({
			.bufferView = (u16)x["bufferView"].asUInt(),
			.componentType = (u16)x["componentType"].asUInt(),
			.count = x["count"].asUInt(),
			.type = x["type"].asString()
		});
	}

	f->scene = src["scene"].asUInt();

	for (auto x: src["scenes"])
	{
		Scene s;
		for (auto y: x["nodes"])
		{
			s.nodes.push_back(y.asUInt());
		}
		f->scenes.push_back(s);
	}

	for (auto x: src["nodes"])
	{
		Node n {
			.children = {},
			.mesh = U8_MAX,
			.skin = U8_MAX,
			.name = x["name"].asString(),
			.rotation = glm::quat(),
			.translation = glm::vec3(),
			.scale = glm::vec3()
		};
		if (!x["mesh"].isNull()) n.mesh = x["mesh"].asUInt();
		if (!x["skin"].isNull()) n.skin = x["skin"].asUInt();
		if (x["rotation"].isArray())
		{
			auto r = x["rotation"];
			n.rotation = glm::quat(
				r[3].asFloat(), r[0].asFloat(),
				r[1].asFloat(), r[2].asFloat()
			);
		}
		if (x["translation"].isArray())
		{
			auto t = x["translation"];
			n.translation = glm::vec3(
				t[0].asFloat(), t[1].asFloat(), t[2].asFloat()
			);
		}
		if (x["scale"].isArray())
		{
			auto s = x["scale"];
			n.scale = glm::vec3(
				s[0].asFloat(), s[1].asFloat(), s[2].asFloat()
			);
		}
		for (auto y: x["children"])
		{
			n.children.push_back(y.asUInt());
		}
		f->nodes.push_back(n);
	}
	
	for (auto x: src["meshes"])
	{
		auto p = x["primitives"][0];
		auto a = p["attributes"];
		f->meshes.push_back({
			.name = x["name"].asString(),
			.vertices = (u16)a["POSITION"].asUInt(),
			.normal = (u16)a["NORMAL"].asUInt(),
			.texCoord = a["TEXCOORD_0"].isNull() ? U16_MAX : (u16)a["TEXCOORD_0"].asUInt(),
			.joints = a["JOINTS_0"].isNull() ? U16_MAX : (u16)a["JOINTS_0"].asUInt(),
			.weights = a["WEIGHTS_0"].isNull() ? U16_MAX : (u16)a["WEIGHTS_0"].asUInt(),
			.indices = (u16)p["indices"].asUInt(),
			.material = p["material"].isNull() ? U8_MAX : (u8)p["material"].asUInt()
		});
	}

	for (auto x: src["materials"])
	{
		f->materials.push_back({
			.name = x["name"].asString(),
			.texture = (u8)x["pbrMetallicRoughness"]
				["baseColorTexture"]["index"].asUInt()
		});
	}

	for (auto x: src["textures"])
	{
		f->textures.push_back({
			.sampler = (u8)x["sampler"].asUInt(),
			.source = (u8)x["source"].asUInt()
		});
	}

	for (auto x: src["samplers"])
	{
		f->samplers.push_back({
			.magFilter = (u16)x["magFilter"].asUInt(),
			.minFilter = (u16)x["minFilter"].asUInt(),
			.wrapS = (u16)x["wrapS"].asUInt(),
			.wrapT = (u16)x["wrapT"].asUInt()
		});
	}

	for (auto x: src["images"])
	{
		f->images.push_back({
			.mimeType = x["mimeType"].asString(),
			.name = x["name"].asString(),
			.uri = x["uri"].asString()
		});
	}

	for (auto x: src["skins"])
	{
		std::vector<u16> joints;
		for (auto y: x["joints"]) joints.push_back(y.asUInt());
		f->skins.push_back({
			.inverseBindMatrices = (u8)x["inverseBindMatrices"].asUInt(),
			.joints = joints,
			.name = x["name"].asString()
		});
	}

	for (auto x: src["animations"])
	{
		// TODO count of channels and animations show 0/0
		std::vector<Animation::Channel> channels;
		std::vector<Animation::Sampler> samplers;
		for (auto c: x["channels"])
		{
			using TP = Animation::Channel::TargetPath;
			TP tp;
			auto raw = c["target"]["path"].asString();
			if (raw == "translation") tp = TP::Translation;
			if (raw == "rotation") tp = TP::Rotation;
			if (raw == "scale") tp = TP::Scale;
			channels.push_back({
				.sampler = (u16)c["sampler"].asUInt(),
				.targetNode = (u16)c["target"]["node"].asUInt(),
				.targetPath = tp
			});
		}
		for (auto s: x["samplers"])
		{
			using IP = ae::anim::Interpolation;
			IP ip;
			auto raw = s["interpolation"].asString();
			if (raw == "STEP") ip = IP::Step;
			if (raw == "LINEAR") ip = IP::Linear;
			if (raw == "CUBICSPLINE") ip = IP::CubicSpline;
			samplers.push_back({
				.input = (u16)s["input"].asUInt(),
				.interpolation = ip,
				.output = (u16)s["output"].asUInt()
			});
		}
		Animation a {
			.channels = channels,
			.duration = x["extras"]["duration"].asFloat(),
			.name = x["name"].asString(),
			.samplers = samplers,
			.repeat = x["extras"]["repeat"].asBool()
		};
		f->animations.push_back(a);
	}

	return f;
}