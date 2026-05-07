#include <ae/global.hpp>
#include <ae/gltf.hpp>
using namespace ae::gltf;

#include <nlohmann/json.hpp>

GLTF* ae::gltf::load(const char* id)
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
		std::string file = x["uri"];
		auto uri = ae::str::format("res/meshes/%s", file.c_str());
		u32 len = x["byteLength"];
		auto [length, data] = ae::fs::readBinary(uri);
		if (len != length)
		{
			printf("WARNING: GLTF \"%s\": Lengths are not the same(%i|%u)\n",
				file.c_str(), len, length
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
			.buffer = x["buffer"],
			.byteOffset = x["byteOffset"],
			.byteLength = x["byteLength"],
			.target = u16(x["target"].is_number() ? (u16)x["target"] : 0)
		});
	}

	for (auto x: src["accessors"])
	{
		f->accessors.push_back({
			.bufferView = x["bufferView"],
			.componentType = x["componentType"],
			.count = x["count"],
			.type = x["type"]
		});
	}

	f->scene = src["scene"];

	for (auto x: src["scenes"])
	{
		Scene s;
		for (auto y: x["nodes"])
		{
			s.nodes.push_back(y);
		}
		f->scenes.push_back(s);
	}

	for (auto x: src["nodes"])
	{
		Node n {
			.children = {},
			.mesh = u8max,
			.skin = u8max,
			.name = x["name"],
			.rotation = glm::quat(),
			.translation = glm::vec3(),
			.scale = glm::vec3()
		};
		if (!x["mesh"].is_null()) n.mesh = x["mesh"];
		if (!x["skin"].is_null()) n.skin = x["skin"];
		if (x["rotation"].is_array())
		{
			auto r = x["rotation"];
			n.rotation = glm::quat(r[3], r[0], r[1], r[2]);
		}
		if (x["translation"].is_array())
		{
			auto t = x["translation"];
			n.translation = glm::vec3(t[0], t[1], t[2]);
		}
		if (x["scale"].is_array())
		{
			auto s = x["scale"];
			n.scale = glm::vec3(s[0], s[1], s[2]);
		}
		for (auto y: x["children"])
		{
			n.children.push_back(y);
		}
		f->nodes.push_back(n);
	}
	
	for (auto x: src["meshes"])
	{
		auto p = x["primitives"][0];
		auto a = p["attributes"];
		f->meshes.push_back({
			.name = x["name"],
			.vertices = a["POSITION"].is_null() ? u16max : (u16)a["POSITION"],
			.normal = a["NORMAL"].is_null() ? u16max : (u16)a["NORMAL"],
			.texCoord = a["TEXCOORD_0"].is_null() ? u16max : (u16)a["TEXCOORD_0"],
			.joints = a["JOINTS_0"].is_null() ? u16max : (u16)a["JOINTS_0"],
			.weights = a["WEIGHTS_0"].is_null() ? u16max : (u16)a["WEIGHTS_0"],
			.indices = p["indices"].is_null() ? u16max : (u16)p["indices"],
			.material = p["material"].is_null() ? u8max : (u8)p["material"]
		});
	}

	for (auto x: src["materials"])
	{
		f->materials.push_back({
			.name = x["name"],
			.texture = x["pbrMetallicRoughness"]
				["baseColorTexture"]["index"]
		});
	}

	for (auto x: src["textures"])
	{
		f->textures.push_back({
			.sampler = x["sampler"],
			.source = x["source"]
		});
	}

	for (auto x: src["samplers"])
	{
		f->samplers.push_back({
			.magFilter = x["magFilter"],
			.minFilter = x["minFilter"],
			.wrapS = u16(x["wrapS"].is_number() ? (u16)x["wrapS"] : 0),
			.wrapT = u16(x["wrapT"].is_number() ? (u16)x["wrapT"] : 0)
		});
	}

	for (auto x: src["images"])
	{
		f->images.push_back({
			.mimeType = x["mimeType"],
			.name = x["name"],
			.uri = x["uri"]
		});
	}

	for (auto x: src["skins"])
	{
		std::vector<u16> joints;
		for (auto y: x["joints"]) joints.push_back(y);
		f->skins.push_back({
			.inverseBindMatrices = x["inverseBindMatrices"],
			.joints = joints,
			.name = x["name"]
		});
	}

	for (auto x: src["animations"])
	{
		std::vector<Animation::Channel> channels;
		std::vector<Animation::Sampler> samplers;
		for (auto c: x["channels"])
		{
			using TP = Animation::Channel::TargetPath;
			TP tp;
			auto raw = c["target"]["path"];
			if (raw == "translation") tp = TP::Translation;
			if (raw == "rotation") tp = TP::Rotation;
			if (raw == "scale") tp = TP::Scale;
			channels.push_back({
				.sampler = c["sampler"],
				.targetNode = c["target"]["node"],
				.targetPath = tp
			});
		}
		for (auto s: x["samplers"])
		{
			Interpolation ip;
			auto raw = s["interpolation"];
			if (raw == "STEP") ip = Interpolation::Step;
			if (raw == "LINEAR") ip = Interpolation::Linear;
			if (raw == "CUBICSPLINE") ip = Interpolation::CubicSpline;
			samplers.push_back({
				.input = s["input"],
				.interpolation = ip,
				.output = s["output"]
			});
		}
		Animation a {
			.channels = channels,
			.duration = x["extras"]["duration"],
			.name = x["name"],
			.samplers = samplers,
			.repeat = x["extras"]["repeat"]
		};
		f->animations.push_back(a);
	}

	return f;
}

ae::f32 ae::gltf::applyInterpolation(Interpolation ip, f32 t)
{
	switch (ip)
	{
		case Interpolation::Step: return 0;
		case Interpolation::Linear: return t;
		case Interpolation::CubicSpline: return t < 0.5 ?
		(4 * pow(t, 3)) :
		(1 - pow(-2 * t + 2, 3) * 0.5);
	}
	return 0;
}