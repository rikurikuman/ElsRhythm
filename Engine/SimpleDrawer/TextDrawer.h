#pragma once
#include "Texture.h"
#include "GraphicsPipeline.h"
#include <mutex>

struct FontTexture {
	Texture texture;
	TEXTMETRIC tm;
	GLYPHMETRICS gm;
};

class TextDrawer
{
public:
	static TextDrawer* GetInstance() {
		static TextDrawer instance;
		return &instance;
	}

	static FontTexture GetFontTexture(std::string glyph, std::string fontTypeFace, UINT fontSize, bool useAlign = false);
	static FontTexture GetFontTexture(std::wstring glyph, std::wstring fontTypeFace, UINT fontSize, bool useAlign = false);

	static TextureHandle CreateStringTexture(std::string text, std::string fontTypeFace, UINT fontSize, std::string handle = "");

	static bool LoadFontFromFile(std::string path);

	//static void DrawString(int x, int y, std::string text, std::string fontTypeFace, UINT fontSize);

	GraphicsPipeline pipeline;

private:
	struct Glyph {
		std::wstring glyph;
		std::wstring fontTypeFace;
		UINT fontSize;

		bool operator==(const Glyph& a) const {
			return glyph == a.glyph && fontTypeFace == a.fontTypeFace && fontSize == a.fontSize;
		}

		bool operator<(const Glyph& a) const {
			if (glyph < a.glyph) {
				return true;
			}
			else if (glyph == a.glyph) {
				if (fontTypeFace < a.fontTypeFace) {
					return true;
				}
				else if (fontTypeFace == a.fontTypeFace) {
					return fontSize < a.fontSize;
				}
			}

			return false;
		}
	};

	TextDrawer() {
		Init();
	};
	~TextDrawer() = default;
	TextDrawer(const TextDrawer& a) = delete;
	TextDrawer& operator=(const TextDrawer&) = delete;

	void Init();

	std::recursive_mutex mutex;
	std::map<Glyph, FontTexture> glyphMap; //�O���t�e�N�X�`���̃}�b�v
};

