#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"



class ImageMachinePart {
public:
	ImageMachinePart() : _output(nullptr) {}


	virtual olc::Sprite* process(const olc::Sprite* input) {
		return outputSprite(1, 1);
	}

	olc::Sprite* outputSprite(int w, int h) {
		if (_output == nullptr) {
			_output = new olc::Sprite(w, h);
		}
		return _output;
	}

	virtual ~ImageMachinePart() {
		if (_output != nullptr) delete _output;
	}

private:
	olc::Sprite* _output;
};

class MirrorPart : public ImageMachinePart {
public:
	olc::Sprite* process(const olc::Sprite* input) override
	{
		olc::Sprite* output = outputSprite(input->width, input->height);
		for (int x = 0; x < input->width; x++)
			for (int y = 0; y < input->height; y++)
				output->SetPixel(input->width - 1 - x, y, input->GetPixel(x, y));

		return output;
	}
};

class ShrinkPart : public ImageMachinePart {
public:
	olc::Sprite* process(const olc::Sprite* input) override
	{
		olc::Sprite* output = outputSprite(input->width / 2, input->height / 2);
		for (int x = 0; x < output->width; x++)
			for (int y = 0; y < output->height; y++)
				output->SetPixel(x, y, input->GetPixel(x * 2, y * 2));

		return output;
	}

};

class CutPart : public ImageMachinePart {
public:
	olc::Sprite* process(const olc::Sprite* input) override
	{
		olc::Sprite* output = outputSprite(input->width, input->height / 2);
		for (int x = 0; x < output->width; x++)
			for (int y = 0; y < output->height; y++)
				output->SetPixel(x, y, input->GetPixel(x, y));

		return output;
	}
};

enum MachineParts {
	MIRROR,
	SHRINK,
	CUT
};

class ImageMachine : public olc::PixelGameEngine
{
public:
	ImageMachine()
	{
		sAppName = "ImageMachine";
	}	

public:

	olc::Sprite img;
	std::vector<MachineParts> parts;

	std::vector<std::shared_ptr<ImageMachinePart>> machineParts;

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		img.LoadFromFile("tslogo-big.png");

		parts.push_back(MIRROR);
		parts.push_back(CUT);

		createMachineParts();

		return true;
	}

	void createMachineParts() {
		machineParts.clear();
		for (auto p : parts) {
			switch (p) {
			case MIRROR:
				machineParts.push_back(std::make_shared<MirrorPart>());
				break;
			case SHRINK:
				machineParts.push_back(std::make_shared<ShrinkPart>());
				break;
			case CUT:
				machineParts.push_back(std::make_shared<CutPart>());
				break;
			}
		}
	}

	float t = 0;
	bool go = false;

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame	
		int x = t * 100;
		int totalW = 0;

		char txt[256];
		sprintf(txt, "x = %d", x);

		Clear(olc::Pixel(200, 200, 200));

		DrawString(200, 200, txt);

		olc::Sprite* output = &img;
		if (x < output->width) {
			DrawPartialSprite(x, 0, output, x, 0, output->width - x, output->height);
		}
		for (auto p : machineParts) {			
			int w = output->width;			
			totalW += w; 
			
			output = p->process(output);
			int partX = x - totalW;
			if (partX >= 0 && partX < w) {
				DrawPartialSprite(x, 0, output, partX, 0, w - partX, output->height);
			}
			else if (partX < 0 && partX + w >= 0) {
				DrawPartialSprite(totalW, 0, output, 0, 0, partX + w, output->height);
			}
		}

		if (x >= totalW) go = false;

		if (go) t += fElapsedTime;

		if (GetKey(olc::Key::SPACE).bPressed) go = true;
		if (GetKey(olc::Key::ESCAPE).bPressed) return false;

		return true;
	}


#undef MAZE

};


int main()
{
	ImageMachine demo;
	if (demo.Construct(800, 600, 1, 1))
		demo.Start();

	return 0;
}
