#include <ae/gltf.hpp>
#include <ae/global.hpp>

using namespace ae::gltf;

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
			Interpolation ip;
			auto raw = s["interpolation"].asString();
			if (raw == "STEP") ip = Interpolation::Step;
			if (raw == "LINEAR") ip = Interpolation::Linear;
			if (raw == "CUBICSPLINE") ip = Interpolation::CubicSpline;
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